#include "bootloader.h"
#include "ui_bootloader.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QStringList>
#include <iostream>
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QFileDialog>
#include <QThread>

#define CHUNK_SIZE_WORDS    128

Bootloader::Bootloader(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Bootloader){

    ui->setupUi(this);

    this->UIUnlock(false);

    this->TimeoutTimer = new QTimer(this);
    this->TimeoutTimer->setSingleShot(true);
    connect(this->TimeoutTimer, &QTimer::timeout, this, &Bootloader::Timeout);

    this->DevicePingTimer = new QTimer(this);
    connect(this->DevicePingTimer, &QTimer::timeout, this, &Bootloader::DevicePing);

    this->DeviceCheckTimer = new QTimer(this);
    connect(this->DeviceCheckTimer, &QTimer::timeout, this, &Bootloader::C_PingSilent);

    ui->label_detect->setText("Scanning Ports");

    connect(ui->pushButton_ping,        &QPushButton::released, this, &Bootloader::C_Ping);
    connect(ui->pushButton_jump,        &QPushButton::released, this, &Bootloader::C_Jump);
    connect(ui->pushButton_erase,       &QPushButton::released, this, &Bootloader::C_Erase);
    connect(ui->pushButton_verify,      &QPushButton::released, this, &Bootloader::C_Verify);
    connect(ui->pushButton_read,        &QPushButton::released, this, &Bootloader::C_Read);
    connect(ui->pushButton_write,       &QPushButton::released, this, &Bootloader::C_Write);
    connect(ui->pushButton_choosefile,  &QPushButton::released, this, &Bootloader::BrowseFile);

    this->DevicePingTimer->start(500);
}

Bootloader::~Bootloader()
{
    delete this->TimeoutTimer;
    delete this->Serial;
    delete ui;
}

// Ping Button
void Bootloader::C_Ping(void){
    if (!(this->SerialLock.Lock())){
        return;
    }
    ConsoleBasic("Ping");
    MsgHeader header(this->PING, 0);

    this->data_awaited = this->PING_AWAIT_SIZE;
    this->Serial->write(header.SetRawFromHeader());
    this->TimeoutTimer->start(50);

    this->UIUnlock(false);
}

// For Device Check Ping
void Bootloader::C_PingSilent(void){
    if (!(this->SerialLock.Lock())){
        return;
    }
    qDebug() << "Ping Silent";
    this->console_enabled = false;
    MsgHeader header(this->PING, 0);

    this->data_awaited = this->PING_AWAIT_SIZE;
    this->Serial->write(header.SetRawFromHeader());
    this->TimeoutTimer->start(50);

    this->UIUnlock(false);
}

// Erase Button
void Bootloader::C_Erase(void){
    if (!(this->SerialLock.Lock())){
        return;
    }
    ConsoleBasic("Erase");
    MsgHeader header(this->ERASE, 1);
    header.payload.append(ui->comboBox_partition->currentIndex());

    this->data_awaited = this->ERASE_AWAIT_SIZE;
    this->Serial->write(header.SetRawFromHeader());
    this->TimeoutTimer->start(2500);

    this->UIUnlock(false);
}

// Jump Button
void Bootloader::C_Jump(void){
    if (!(this->SerialLock.Lock())){
        return;
    }
    ConsoleBasic("Jumping to Program " + ui->comboBox_partition->currentText());
    MsgHeader header(this->JUMP, 1);
    header.payload.append(ui->comboBox_partition->currentIndex());

    this->data_awaited = this->JUMP_AWAIT_SIZE;
    this->Serial->write(header.SetRawFromHeader());
    this->TimeoutTimer->start(100);
    this->UIUnlock(false);
}

// Read Button
void Bootloader::C_Read(void){
    if (!(this->SerialLock.Lock())){
        return;
    }
    ConsoleBasic("Reading From");
}

// Verify Button
void Bootloader::C_Verify(void){
    if (!(this->SerialLock.Lock())){
        return;
    }
    ConsoleBasic("Firmware Verification Started");
}

// Write Button
void Bootloader::C_Write(void){
//    if (!(this->SerialLock.Lock()))
//        return;

    ConsoleBasic("Write Firmware Started");
    this->file = new QFile(ui->lineEdit_filename->text());

    if (!(this->file->open(QFile::ReadOnly))){
        ConsoleError("Unable to Open Firmware File");
        delete this->file;
        return;
    }
    if (((ui->comboBox_partition->currentIndex() == 0) && (this->file->size() > FLASH_MAP_APP_1)) ||
        ((ui->comboBox_partition->currentIndex() == 1) && (this->file->size() > FLASH_MAP_APP_2)) ||
        ((ui->comboBox_partition->currentIndex() == 2) && (this->file->size() > FLASH_MAP_APP_USER))){
            ConsoleError("Firmware Size Error");
            this->file->close();
            delete this->file;
            return;
    }
    this->file->close();
    delete this->file;

    this->flash_after = 1;
    C_Erase();
}

void Bootloader::ProcessIncomingData(void){
    if (this->TimeoutTimer->isActive())
        this->TimeoutTimer->stop();

    QByteArray data = this->Serial->read(this->data_awaited);
    this->data_awaited = 0;
    this->Serial->readAll();

    MsgHeader header;
    header.SetHeaderFromRaw(data);

    switch(header.cmd){
    case(this->PING):{
        uint32_t id = header.payload[0];
        char * name = reinterpret_cast<char *>(&id);
        QString s_name = "";
        s_name += name[0]; s_name += name[1]; s_name += name[2]; s_name += name[3];

        ConsoleBasic(QString("Ping OK, Device ID is ") + s_name);

        if ((s_name != BOOTLOADER_ID) && (s_name != APP_1_ID) && (s_name != APP_2_ID)){
            ConsoleWarning("Unknown Device " + s_name);
            this->Timeout();
            this->SerialLock.Unlock();
            break;
        }

        this->UIUnlock(true);
        this->console_enabled = true;

        emit siSendSerial(this->Serial);
        if (this->DevicePingTimer->isActive()){
            this->DevicePingTimer->stop();
            this->DeviceCheckTimer->start(1000);
        }

        if (s_name == APP_1_ID){
            disconnect(this->Serial, &QSerialPort::readyRead, this, &Bootloader::PushDataFromStream);
            if (this->DeviceCheckTimer->isActive()) this->DeviceCheckTimer->stop();
            emit siChooseTab(APP_1_TAB);
        }
        else if (s_name == APP_2_ID){
            disconnect(this->Serial, &QSerialPort::readyRead, this, &Bootloader::PushDataFromStream);
            if (this->DeviceCheckTimer->isActive()) this->DeviceCheckTimer->stop();
            emit siChooseTab(APP_2_TAB);
        }
        this->SerialLock.Unlock();
        break;
    }
    case(this->JUMP):{
        ConsoleBasic("Jump to " + ui->comboBox_partition->currentText() + " Completed Successfully");
        this->UIUnlock(true);
        this->SerialLock.Unlock();
        break;
    }
    case(this->VERIFY):{
        this->SerialLock.Unlock();
        break;
    }
    case(this->ERASE):{
        ConsoleBasic("Erasing Partition " + ui->comboBox_partition->currentText() + " Completed");
        if (!(this->flash_after)){
            this->UIUnlock(true);
            this->SerialLock.Unlock();
            break;
        }
        else{
            this->flash_after = 0;
            this->file = new QFile(ui->lineEdit_filename->text());
            this->file->open(QFile::ReadOnly);
        }
    }
    case(this->WRITE):{
        char chunk[CHUNK_SIZE_WORDS * 4];
        uint32_t chunk_size = this->file->read(chunk, CHUNK_SIZE_WORDS * 4);
        if (chunk_size){
            ConsoleBasic("Writing a Chunk of " + QString::fromStdString(std::to_string(chunk_size)) + " Bytes");
            MsgHeader msg(this->WRITE, 3 + chunk_size / 4);
            msg.payload.append(ui->comboBox_partition->currentIndex());
            msg.payload.append(chunk_size / 4);
            msg.payload.append(0 /*CRC here*/);
            for (uint32_t iter = 0; iter < chunk_size; iter += 4){
                msg.payload.append(*((uint32_t *)(chunk + iter)));
            }
            this->data_awaited = this->WRITE_AWAIT_SIZE;
            qDebug() << "Bytes Written : " << this->Serial->write(msg.SetRawFromHeader());
            this->TimeoutTimer->start(1000);
        }
        else{
            this->UIUnlock(true);
            this->SerialLock.Unlock();
            ConsoleBasic("Writing Ended! Jolly Good!");
            this->file->close();
            delete this->file;
        }
        break;
    }
    case(this->READ):{
        this->SerialLock.Unlock();
        break;
    }
    case(this->INTERNAL_ERROR):{
        ConsoleError("Internal Error Occured");
        this->UIUnlock(true);
        this->SerialLock.Unlock();
        break;
    }
    case(this->CRC_ERROR):{
        ConsoleError("CRC Error");
        this->UIUnlock(true);
        this->SerialLock.Unlock();
        break;
    }
    case(this->CMD_ERROR):{
        ConsoleError("Wrong Command for Device");
        this->UIUnlock(true);
        this->SerialLock.Unlock();
        break;
    }
    case(this->PARTITION_ERROR):{
        ConsoleError("Wrong Partition");
        this->UIUnlock(true);
        this->SerialLock.Unlock();
        break;
    }
    case(this->LENGTH_ERROR):{
        ConsoleError("Length Error");
        this->UIUnlock(true);
        this->SerialLock.Unlock();
        break;
    }
    case(this->DATA_ERROR):{
        ConsoleError("Wrong Data");
        this->UIUnlock(true);
        this->SerialLock.Unlock();
        break;
    }
    default:{
        ConsoleError("Unknown Error");
        this->UIUnlock(true);
        this->SerialLock.Unlock();
        break;
    }
    }
}

void Bootloader::ErrorCatch(uint32_t error_code){
    std::cout << "Error Code" << error_code << std::endl;
}

void Bootloader::UIUnlock(bool lock){
    ui->pushButton_choosefile->setEnabled(lock);
    ui->pushButton_erase->setEnabled(lock);
    ui->pushButton_jump->setEnabled(lock);
    ui->pushButton_ping->setEnabled(lock);
    ui->pushButton_read->setEnabled(lock);
    ui->pushButton_verify->setEnabled(lock);
    ui->pushButton_write->setEnabled(lock);
    ui->comboBox_partition->setEnabled(lock);
}

void Bootloader::PushDataFromStream(void){
    if ((this->data_awaited) && (this->Serial->bytesAvailable() >= this->data_awaited))
        this->ProcessIncomingData();
}

void Bootloader::ConsoleBasic(QString message){
    if (!(this->console_enabled)) return;
    ui->listWidget_debugconsole->addItem("[INFO] " + message);
    ui->listWidget_debugconsole->scrollToBottom();
}

void Bootloader::ConsoleError(QString message){
    if (!(this->console_enabled)) return;
    ui->listWidget_debugconsole->addItem("[WARNING] " + message);
    ui->listWidget_debugconsole->item(ui->listWidget_debugconsole->count() - 1)->setForeground(QColor(219, 169, 0));
    ui->listWidget_debugconsole->scrollToBottom();
}

void Bootloader::ConsoleWarning(QString message){
    if (!(this->console_enabled)) return;
    ui->listWidget_debugconsole->addItem("[ERROR] " + message);
    ui->listWidget_debugconsole->item(ui->listWidget_debugconsole->count() - 1)->setForeground(QColor(208, 51, 51));
    ui->listWidget_debugconsole->scrollToBottom();
}

void Bootloader::BrowseFile(void){
    QString fname = QFileDialog::getOpenFileName(this,
                                                 tr("Open Firmware File"),
                                                 "/home/egor/",
                                                 tr("Binary Files (*.bin)"));
    ui->lineEdit_filename->setText(fname);
}

void Bootloader::DevicePing(void){
    if (this->available_ports.isEmpty()){
        this->available_ports = QSerialPortInfo::availablePorts();
    }
    if (this->Serial != nullptr){
        delete this->Serial;
    }
    this->Serial = new QSerialPort;
    this->Serial->setPortName(this->available_ports.takeFirst().portName());
    this->Serial->setBaudRate(QSerialPort::Baud57600);
    if (!(this->Serial->open(QIODevice::ReadWrite))){
        delete this->Serial;
        return;
    }

    ui->label_detect->setText("Active Port at | " + this->Serial->portName() + " |");

    connect(this->Serial, &QSerialPort::readyRead, this, &Bootloader::PushDataFromStream);
    this->C_Ping();

}
