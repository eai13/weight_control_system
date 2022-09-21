#include "systemcontrol.h"
#include "ui_systemcontrol.h"
#include "qcustomplot.h"
#include "plot3dconfigs.h"
#include <QVector>
#include <QMenu>
#include <QCursor>
#include <QAction>
#include <QActionGroup>

// tmp

#include <iostream>

SystemControl::SystemControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemControl)
{
    ui->setupUi(this);

    this->plot3d = new Plot3D(ui->groupBox_plot3d);
    this->plot3d->AddRealPoint(50, 50, 50);
    this->plot3d->BuildTargetTrajectory(QVector3D(0, 0, 0), QVector3D(10, 1, 1));
    this->plot3d->BuildTargetTrajectory(QVector3D(10, 1, 1), QVector3D(25, 16, 16));

    Plot3DConfigs * plot3dconfigs = new Plot3DConfigs();
    ui->groupBox_3dplot_settings->layout()->addWidget(plot3dconfigs);
    connect(plot3dconfigs, &Plot3DConfigs::siFullscreen, this->plot3d, &Plot3D::slFullscreen);
    connect(plot3dconfigs, &Plot3DConfigs::siTargetAdd, this->plot3d, &Plot3D::slTargetAdd);
    connect(plot3dconfigs, &Plot3DConfigs::siTargetClear, this->plot3d, &Plot3D::slTargetClear);
    connect(plot3dconfigs, &Plot3DConfigs::siTargetRemove, this->plot3d, &Plot3D::slTargetRemove);
    connect(plot3dconfigs, &Plot3DConfigs::siStartTrajectory, this, &SystemControl::slStartTrajectory);
    connect(plot3dconfigs, &Plot3DConfigs::siStartTrajectory, this->plot3d, &Plot3D::slStartTrajectory);
    connect(plot3dconfigs, &Plot3DConfigs::siStopTrajectory, this, &SystemControl::slStopTrajectory);
    connect(plot3dconfigs, &Plot3DConfigs::siStopTrajectory, this->plot3d, &Plot3D::slStopTrajectory);
    connect(plot3dconfigs, &Plot3DConfigs::siPauseTrajectory, this, &SystemControl::slPauseTrajectory);
    connect(plot3dconfigs, &Plot3DConfigs::siPauseTrajectory, this->plot3d, &Plot3D::slPauseTrajectory);
    connect(plot3dconfigs, &Plot3DConfigs::siSaveTarget, this->plot3d, &Plot3D::slSaveTarget);
    connect(plot3dconfigs, &Plot3DConfigs::siSaveReal, this->plot3d, &Plot3D::slSaveReal);
    connect(plot3dconfigs, &Plot3DConfigs::siUploadTarget, this->plot3d, &Plot3D::slUploadTarget);
    this->SystemTime = new QTime();
    this->SystemTime->start();

    this->plots[0] = new Plot2D(ui->widget_plot1, ui->radioButton_m1turns, ui->radioButton_m1rads, ui->dial_motor1, ui->lineEdit_motor1pos);
    this->plots[1] = new Plot2D(ui->widget_plot2, ui->radioButton_m2turns, ui->radioButton_m2rads, ui->dial_motor2, ui->lineEdit_motor2pos);
    this->plots[2] = new Plot2D(ui->widget_plot3, ui->radioButton_m3turns, ui->radioButton_m3rads, ui->dial_motor3, ui->lineEdit_motor3pos);
    this->plots[3] = new Plot2D(ui->widget_plot4, ui->radioButton_m4turns, ui->radioButton_m4rads, ui->dial_motor4, ui->lineEdit_motor4pos);

    for (uint8_t iter = 0; iter < 4; iter++){
        this->plots[iter]->setObjectName(QString::fromStdString(std::to_string(iter)));
        connect(this->plots[iter], &Plot2D::siSendPos, this, &SystemControl::slSendPos);
    }

    this->PlottableDataTimer = new QTimer(this);
    connect(this->PlottableDataTimer, &QTimer::timeout, this, &SystemControl::C_ReadPlottableRegs);

    this->TransmitHandlerTimer = new QTimer(this);
    connect(this->TransmitHandlerTimer, &QTimer::timeout, this, &SystemControl::SerialTxHandler);

    this->TimeoutTimer = new QTimer(this);
    this->TimeoutTimer->setSingleShot(true);
    connect(this->TimeoutTimer, &QTimer::timeout, this, &SystemControl::Timeout);

    this->DeviceCheckTimer = new QTimer(this);
    connect(this->DeviceCheckTimer, &QTimer::timeout, this, &SystemControl::C_PingSilent);

    connect(ui->pushButton_quitapp, &QPushButton::released, this, &SystemControl::C_Quit);

}

void SystemControl::C_PingSilent(void){
    BP_Header header(this->BP_PING, 0);

    Packet pack(header.SetRawFromHeader(), this->BP_PING_AWAIT_SIZE, this->PERIODTimeoutTimer);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_Quit(void){
    BP_Header header(this->BP_JUMP, 1);
    header.payload.append(0);

    Packet pack(header.SetRawFromHeader(), this->BP_JUMP_AWAIT_SIZE, this->PERIODTimeoutTimer);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_ReadMultipleData(uint16_t reg){
    QVector<float> tmp_data; tmp_data.append(1); tmp_data.append(2); tmp_data.append(3); tmp_data.append(4);
    BP_Header       bp_header(this->BP_CONTROL, 7);
    CNT_Header      cnt_header(CNT_ID_GLOBAL, CNT_READ_REG);
    CNT_Register    cnt_register(reg, tmp_data);

    QByteArray b_data = bp_header.SetRawFromHeader();
    b_data.append(cnt_header.SetRawFromHeader());
    b_data.append(cnt_register.SetRawFromHeader());

    Packet pack(b_data, this->BP_CNT_READ_MULTIPLE_AWAIT_SIZE, this->PERIODTimeoutTimer);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_ReadSingleData(uint16_t id, uint16_t reg){
    QVector<float> tmp_data; tmp_data.append(0);
    BP_Header       bp_header(this->BP_CONTROL, 4);
    CNT_Header      cnt_header(id, CNT_READ_REG);
    CNT_Register    cnt_register(reg, tmp_data);

    QByteArray b_data = bp_header.SetRawFromHeader();
    b_data.append(cnt_header.SetRawFromHeader());
    b_data.append(cnt_register.SetRawFromHeader());

    Packet pack(b_data, this->BP_CNT_READ_SINGLE_AWAIT_SIZE, this->PERIODTimeoutTimer);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_WriteMultipleData(uint16_t reg, QVector<float> data){
    BP_Header       bp_header(this->BP_CONTROL, 7);
    CNT_Header      cnt_header(CNT_ID_GLOBAL, CNT_WRITE_REG);
    CNT_Register    cnt_register(reg, data);

    QByteArray b_data = bp_header.SetRawFromHeader();
    b_data.append(cnt_header.SetRawFromHeader());
    b_data.append(cnt_register.SetRawFromHeader());

    Packet pack(b_data, this->BP_CNT_WRITE_MULTIPLE_AWAIT_SIZE, this->PERIODTimeoutTimer);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_WriteSingleData(uint16_t id, uint16_t reg, float data){
    QVector<float>  tmp_data; tmp_data.append(data);
    BP_Header       bp_header(this->BP_CONTROL, 4);
    CNT_Header      cnt_header(id, CNT_WRITE_REG);
    CNT_Register    cnt_register(reg, tmp_data);

    QByteArray b_data = bp_header.SetRawFromHeader();
    b_data.append(cnt_header.SetRawFromHeader());
    b_data.append(cnt_register.SetRawFromHeader());

    Packet pack(b_data, this->BP_CNT_WRITE_SINGLE_AWAIT_SIZE, this->PERIODTimeoutTimer);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_ReadPlottableRegs(void){
    BP_Header       bp_header(this->BP_CONTROL, 2);
    CNT_Header      cnt_header(CNT_ID_GLOBAL, CNT_READ_PLOTTABLE);

    QByteArray b_data = bp_header.SetRawFromHeader();
    b_data.append(cnt_header.SetRawFromHeader());

    Packet pack(b_data, this->BP_CNT_READ_PLOTTABLE_AWAIT_SIZE, 100);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_SendCmd(uint16_t cmd){
    return;
}

SystemControl::~SystemControl()
{
    delete ui;
}

void SystemControl::ProcessIncomingData(void){
    if (this->TimeoutTimer->isActive())
        this->TimeoutTimer->stop();

    QByteArray data = this->Serial->read(this->data_awaited);
    this->data_awaited = 0;
    this->Serial->readAll();

    BP_Header bp_header;
    bp_header.SetHeaderFromRaw(data);

    switch(bp_header.cmd){
    case(this->BP_PING):{
        uint32_t id = bp_header.payload[0];
        char * name = reinterpret_cast<char *>(&id);
        QString s_name = "";
        s_name += name[0]; s_name += name[1]; s_name += name[2]; s_name += name[3];
        qDebug() << "Ping process";
        ConsoleBasic(QString("Ping OK, Device ID is ") + s_name);

        if ((s_name != BOOTLOADER_ID) && (s_name != APP_1_ID) && (s_name != APP_2_ID)){
            ConsoleWarning(("Unknown Device " + s_name));
            this->Exit(BOOTLOADER_TAB);
        }

        emit siSendSerial(this->Serial);

        if (s_name == BOOTLOADER_ID){
            this->Exit(BOOTLOADER_TAB);
        }
        else if (s_name == APP_2_ID){
            this->Exit(APP_2_TAB);
        }

        this->SerialLock.Unlock();
        break;
    }
    case(this->BP_JUMP):{
        ConsoleBasic("Jump back to boot");
        qDebug() << "Jump back";
        this->PlottableDataTimer->stop();
        this->serial_tx_queue.clear();
        this->SerialLock.Unlock();
        break;
    }
    case(this->BP_CONTROL):{
        CNT_Header cnt_header;
        cnt_header.SetHeaderFromBPPayload(&bp_header);
        switch(cnt_header.cmd){
        case(CNT_WRITE_REG):{
            CNT_Register cnt_register;
            cnt_register.SetHeaderFromCNTPayload(&cnt_header);
            switch(cnt_header.id){
            case(CNT_ID_GLOBAL):
                break;
            default:
                qDebug() << cnt_header.cmd << " " << cnt_header.id;
                qDebug() << cnt_register.reg << " " << cnt_register.data;
                break;
            }
            break;
        }
        case(CNT_READ_REG):{
            CNT_Register cnt_register;
            cnt_register.SetHeaderFromCNTPayload(&cnt_header);
            switch(cnt_header.id){
            case(CNT_ID_GLOBAL):
                this->SerialLock.Unlock();
                break;
            default:
                break;
            }
            break;
        }
        case(CNT_BLOCK_DRIVE):
            qDebug() << "Drive Locked";
            break;
        case(CNT_ENABLE_DRIVE):
            qDebug() << "Drive Enabled";
            break;
        case(CNT_READ_PLOTTABLE):{
            CNT_Plottable cnt_plottable;
            cnt_plottable.SetHeaderFromCNTPayload(&cnt_header);
            for (uint8_t iter = 0; iter < 4; iter++){
                this->plots[iter]->slAddData(CNT_REG_POS_SP, cnt_plottable.data[0 + iter * 7]);
                this->plots[iter]->slAddData(CNT_REG_POS_FB, cnt_plottable.data[1 + iter * 7]);
                this->plots[iter]->slAddData(CNT_REG_SPD_SP, cnt_plottable.data[2 + iter * 7]);
                this->plots[iter]->slAddData(CNT_REG_SPD_FB, cnt_plottable.data[3 + iter * 7]);
                this->plots[iter]->slAddData(CNT_REG_CUR_SP, cnt_plottable.data[4 + iter * 7]);
                this->plots[iter]->slAddData(CNT_REG_CUR_FB, cnt_plottable.data[5 + iter * 7]);
                this->plots[iter]->slAddData(CNT_REG_OUTPUT, cnt_plottable.data[6 + iter * 7]);
            }
        }
        }

//        qDebug() << "Control request";
//        qDebug() << "BP:    CMD " << bp_header.cmd << " W_Size " << bp_header.w_size;
//        qDebug() << "CNT:   ID  " << cnt_header.id << " CMD    " << cnt_header.cmd;
//        qDebug() << "REG:   " << cnt_register.reg;
//        qDebug() << "DATA:  " << cnt_register.data[0] << " " << cnt_register.data[1] << " " << cnt_register.data[2] << " " << cnt_register.data[3];
        this->SerialLock.Unlock();
        break;
    }
    default:
        ConsoleError("Unknown Error");
        this->SerialLock.Unlock();
        break;
    }
}

void SystemControl::PushDataFromStream(void){
    if ((this->data_awaited) && (this->Serial->bytesAvailable() >= this->data_awaited))
        this->ProcessIncomingData();
}

void SystemControl::slActivate(void){
    connect(this->Serial, &QSerialPort::readyRead, this, &SystemControl::PushDataFromStream);
    this->DeviceCheckTimer->start(this->PERIODDeviceCheckTimer);
    this->PlottableDataTimer->start(this->PERIODPlotDataTimer);
    this->TransmitHandlerTimer->start(this->PERIODTransmitHandlerTimer);
}

void SystemControl::ConsoleBasic(QString message){
    if (!(this->console_enabled)) return;
    ui->listWidget_debugconsole->addItem("[INFO] " + message);
    ui->listWidget_debugconsole->scrollToBottom();
}

void SystemControl::ConsoleError(QString message){
    if (!(this->console_enabled)) return;
    ui->listWidget_debugconsole->addItem("[WARNING] " + message);
    ui->listWidget_debugconsole->item(ui->listWidget_debugconsole->count() - 1)->setForeground(QColor(219, 169, 0));
    ui->listWidget_debugconsole->scrollToBottom();
}

void SystemControl::ConsoleWarning(QString message){
    if (!(this->console_enabled)) return;
    ui->listWidget_debugconsole->addItem("[ERROR] " + message);
    ui->listWidget_debugconsole->item(ui->listWidget_debugconsole->count() - 1)->setForeground(QColor(208, 51, 51));
    ui->listWidget_debugconsole->scrollToBottom();
}

void SystemControl::slSendPos(float data){
    uint8_t sender_id = sender()->objectName().toInt();
    this->C_WriteSingleData(sender_id, this->CNT_REG_POS_SP, data);
}

void SystemControl::Exit(uint8_t tab){
    disconnect(this->Serial, &QSerialPort::readyRead, this, &SystemControl::PushDataFromStream);
    if (this->DeviceCheckTimer->isActive()) this->DeviceCheckTimer->stop();
    if (this->TransmitHandlerTimer->isActive()) this->TransmitHandlerTimer->stop();
    if (this->PlottableDataTimer->isActive()) this->PlottableDataTimer->stop();
    for (uint8_t iter = 0; iter < 4; iter++)
        this->plots[iter]->ResetPlot();
    emit this->siChooseTab(tab);
}

void SystemControl::slStartTrajectory(void){
    qDebug() << "SystemControl Start Trajectory";
}
void SystemControl::slStopTrajectory(void){
    qDebug() << "SystemControl Stop Trajectory";
}
void SystemControl::slPauseTrajectory(void){
    qDebug() << "SystemControl Pause Trajectory";
}
