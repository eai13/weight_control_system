#include "systemcontrol.h"
#include "ui_systemcontrol.h"
#include "qcustomplot.h"
#include <QVector>
#include <QMenu>
#include <QCursor>
#include <QAction>
#include <QActionGroup>

SystemControl::SystemControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemControl)
{
    ui->setupUi(this);

    this->InitGraphs();

    this->TimeoutTimer = new QTimer(this);
    connect(this->TimeoutTimer, &QTimer::timeout, this, &SystemControl::Timeout);

    this->DeviceCheckTimer = new QTimer(this);
    connect(this->DeviceCheckTimer, &QTimer::timeout, this, &SystemControl::C_PingSilent);


    connect(ui->pushButton_quitapp, &QPushButton::released, this, &SystemControl::C_Quit);
    qDebug() << senderSignalIndex();
}

void SystemControl::C_PingSilent(void){
    if (!(this->SerialLock.Lock())){
        return;
    }
//    qDebug() << "Ping Silent APP1";
    BP_Header header(this->BP_PING, 0);

    this->data_awaited = this->BP_PING_AWAIT_SIZE;
    this->Serial->write(header.SetRawFromHeader());
    this->TimeoutTimer->start(50);
}

void SystemControl::C_Quit(void){
    if (!(this->SerialLock.Lock())){
        return;
    }

    qDebug() << "Quit";
    BP_Header header(this->BP_JUMP, 1);
    header.payload.append(0);

    this->data_awaited = this->BP_JUMP_AWAIT_SIZE;
    this->Serial->write(header.SetRawFromHeader());
    this->TimeoutTimer->start(100);
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
//        qDebug() << "ping receive app1";
        uint32_t id = bp_header.payload[0];
        char * name = reinterpret_cast<char *>(&id);
        QString s_name = "";
        s_name += name[0]; s_name += name[1]; s_name += name[2]; s_name += name[3];

        ConsoleBasic(QString("Ping OK, Device ID is ") + s_name);

        if ((s_name != BOOTLOADER_ID) && (s_name != APP_1_ID) && (s_name != APP_2_ID)){
            ConsoleWarning(("Unknown Device " + s_name));

        }

        emit siSendSerial(this->Serial);

        if (s_name == BOOTLOADER_ID){
            disconnect(this->Serial, &QSerialPort::readyRead, this, &SystemControl::PushDataFromStream);
            if (this->DeviceCheckTimer->isActive()) this->DeviceCheckTimer->stop();
            emit siChooseTab(BOOTLOADER_TAB);
        }
        else if (s_name == APP_2_ID){
            disconnect(this->Serial, &QSerialPort::readyRead, this, &SystemControl::PushDataFromStream);
            if (this->DeviceCheckTimer->isActive()) this->DeviceCheckTimer->stop();
            emit siChooseTab(APP_2_TAB);
        }

        this->SerialLock.Unlock();
        break;
    }
    case(this->BP_JUMP):{
        ConsoleBasic("Jump back to boot");
        this->SerialLock.Unlock();
        break;
    }
    case(this->BP_CONTROL):
        break;
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
    this->DeviceCheckTimer->start(1000);
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

// GRAPHS

void SystemControl::InitGraphs(void){

    this->plot_handles[0] = ui->widget_plot1;
    this->plot_handles[1] = ui->widget_plot2;
    this->plot_handles[2] = ui->widget_plot3;
    this->plot_handles[3] = ui->widget_plot4;

    for (uint8_t iter = 0; iter < 4; iter++){
        this->plot_handles[iter]->clearItems();
        this->plot_handles[iter]->addGraph();
        this->plot_handles[iter]->xAxis->setLabel("Time");
        this->plot_handles[iter]->yAxis->setLabel("Parameter");
    }

    this->plot_context_menu.addAction("Clear", this, &SystemControl::slClearPlots);
    this->plot_context_menu.addAction("Rescale", this, &SystemControl::slRescalePlots);
    this->plot_context_menu.addAction("Auto Rescale", this, &SystemControl::slAutoRescalePlots);

        QMenu * parameter_menu = this->plot_context_menu.addMenu("Parameter");
        parameter_menu->setObjectName(QString("Parameter Menu"));

        parameter_menu->addAction("Torque")->setObjectName(this->RegisterNames[CNT_REG_TORQUE].name);
        parameter_menu->addAction("Position Setpoint")->setObjectName(this->RegisterNames[CNT_REG_POS_SP].name);
        parameter_menu->addAction("Position")->setObjectName(this->RegisterNames[CNT_REG_POS_FB].name);
        parameter_menu->addAction("Position Loop Accumulator")->setObjectName(this->RegisterNames[CNT_REG_POS_ACC].name);
        parameter_menu->addAction("Velocity Setpoint")->setObjectName(this->RegisterNames[CNT_REG_SPD_SP].name);
        parameter_menu->addAction("Velocity")->setObjectName(this->RegisterNames[CNT_REG_SPD_FB].name);
        parameter_menu->addAction("Velocity Loop Accumulator")->setObjectName(this->RegisterNames[CNT_REG_SPD_ACC].name);
        parameter_menu->addAction("Current Setpoint")->setObjectName(this->RegisterNames[CNT_REG_CUR_SP].name);
        parameter_menu->addAction("Current")->setObjectName(this->RegisterNames[CNT_REG_CUR_FB].name);
        parameter_menu->addAction("Current Loop Accumulator")->setObjectName(this->RegisterNames[CNT_REG_CUR_ACC].name);
        parameter_menu->addAction("Motor Output Voltage")->setObjectName(this->RegisterNames[CNT_REG_OUTPUT].name);

        for (auto iter = parameter_menu->actions().begin(); iter != parameter_menu->actions().end(); iter++){
            (*iter)->setCheckable(true);
            connect((*iter), &QAction::triggered, this, &SystemControl::slParameterHandle);
        }

        QMenu * tmp = this->plot_context_menu.findChild<QMenu *>("Parameter Menu");
        for (auto iter = tmp->children().begin(); iter != tmp->children().end(); iter++)
            qDebug() << (*iter)->objectName();

    for (uint8_t iter = 0; iter < 4; iter++){
        this->plot_handles[iter]->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this->plot_handles[iter], &QCustomPlot::customContextMenuRequested, this, &SystemControl::slShowContextMenu);
    }
}

void SystemControl::slClearPlots(void){
    qDebug() << "cleared plots";
    for (uint8_t iter = 0; iter < 4; iter++)
        this->plot_handles[iter]->clearGraphs();
}

void SystemControl::slShowContextMenu(const QPoint & pos){
    qDebug() << senderSignalIndex();
    this->plot_context_menu.exec(QCursor::pos());
}

void SystemControl::slRescalePlots(void){
    qDebug() << "Rescale plots " << sender()->objectName();
}

void SystemControl::slAutoRescalePlots(void){
    qDebug() << "Auto Rescale Plots";
}

void SystemControl::slParameterHandle(bool state){
    QString sender_name = sender()->objectName();
    QMenu * tmp = this->plot_context_menu.findChild<QMenu *>("Parameter Menu");
    if (sender_name == this->RegisterNames[CNT_REG_TORQUE].name){
        this->RegisterNames[CNT_REG_TORQUE].is_active = state;
        for (auto iter = tmp->actions().begin(); iter != tmp->actions().end(); iter++){
            if ((*iter)->objectName() != this->RegisterNames[CNT_REG_TORQUE].name)
                (*iter)->setEnabled(!state);
        }
    }
    if ((sender_name == this->RegisterNames[CNT_REG_POS_SP].name) ||
        (sender_name == this->RegisterNames[CNT_REG_POS_FB].name) ||
        (sender_name == this->RegisterNames[CNT_REG_POS_ACC].name)){

        QString sp = this->RegisterNames[CNT_REG_POS_SP].name;
        QString fb = this->RegisterNames[CNT_REG_POS_FB].name;
        QString acc = this->RegisterNames[CNT_REG_POS_ACC].name;
            if (sender_name == sp) this->RegisterNames[CNT_REG_POS_SP].is_active = state;
            else if (sender_name == fb) this->RegisterNames[CNT_REG_POS_FB].is_active = state;
            else if (sender_name == acc) this->RegisterNames[CNT_REG_POS_ACC].is_active = state;

            if ((!(this->RegisterNames[CNT_REG_POS_SP].is_active)) &&
                (!(this->RegisterNames[CNT_REG_POS_FB].is_active)) &&
                (!(this->RegisterNames[CNT_REG_POS_ACC].is_active)))
                    for (auto iter = tmp->actions().begin(); iter != tmp->actions().end(); iter++){
                        if ((sender_name != sp) && (sender_name != fb) && (sender_name != acc))
                            (*iter)->setEnabled(!state);
                    }
    }
}
