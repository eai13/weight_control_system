#include "systemcontrol.h"
#include "ui_systemcontrol.h"
#include "qcustomplot.h"
#include <QVector>
#include <QMenu>
#include <QCursor>
#include <QAction>
#include <QActionGroup>

uint32_t flag = 0;

SystemControl::SystemControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemControl)
{
    ui->setupUi(this);

    this->SystemTime = new QTime();
    this->SystemTime->start();

    this->PlotDataTimer = new QTimer(this);
    connect(this->PlotDataTimer, &QTimer::timeout, this, &SystemControl::slPlotDataRequest);

    this->TransmitHandlerTimer = new QTimer(this);
    connect(this->TransmitHandlerTimer, &QTimer::timeout, this, &SystemControl::SerialTxHandler);

    this->InitGraphs();

    this->TimeoutTimer = new QTimer(this);
    this->TimeoutTimer->setSingleShot(true);
    connect(this->TimeoutTimer, &QTimer::timeout, this, &SystemControl::Timeout);

    this->DeviceCheckTimer = new QTimer(this);
    connect(this->DeviceCheckTimer, &QTimer::timeout, this, &SystemControl::C_PingSilent);

    connect(ui->pushButton_quitapp, &QPushButton::released, this, &SystemControl::C_Quit);
}

void SystemControl::C_PingSilent(void){
    BP_Header header(this->BP_PING, 0);

    Packet pack(header.SetRawFromHeader(), this->BP_PING_AWAIT_SIZE, 50);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_Quit(void){
    BP_Header header(this->BP_JUMP, 1);
    header.payload.append(0);

    Packet pack(header.SetRawFromHeader(), this->BP_JUMP_AWAIT_SIZE, 100);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_ReadMultipleData(CONTROL_Registers reg){
    QVector<float> tmp_data; tmp_data.append(1); tmp_data.append(2); tmp_data.append(3); tmp_data.append(4);
    BP_Header       bp_header(this->BP_CONTROL, 7);
    CNT_Header      cnt_header(CNT_ID_GLOBAL, CNT_READ_REG);
    CNT_Register    cnt_register(reg, tmp_data);

    QByteArray b_data = bp_header.SetRawFromHeader();
    b_data.append(cnt_header.SetRawFromHeader());
    b_data.append(cnt_register.SetRawFromHeader());

    Packet pack(b_data, this->BP_CNT_READ_MULTIPLE_AWAIT_SIZE, 100);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_ReadSingleData(CONTROL_IDs id, CONTROL_Registers reg){
    QVector<float> tmp_data; tmp_data.append(0);
    BP_Header       bp_header(this->BP_CONTROL, 4);
    CNT_Header      cnt_header(id, CNT_READ_REG);
    CNT_Register    cnt_register(reg, tmp_data);

    QByteArray b_data = bp_header.SetRawFromHeader();
    b_data.append(cnt_header.SetRawFromHeader());
    b_data.append(cnt_register.SetRawFromHeader());

    Packet pack(b_data, this->BP_CNT_READ_SINGLE_AWAIT_SIZE, 100);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_WriteMultipleData(CONTROL_Registers reg, QVector<float> data){
    BP_Header       bp_header(this->BP_CONTROL, 7);
    CNT_Header      cnt_header(CNT_ID_GLOBAL, CNT_WRITE_REG);
    CNT_Register    cnt_register(reg, data);

    QByteArray b_data = bp_header.SetRawFromHeader();
    b_data.append(cnt_header.SetRawFromHeader());
    b_data.append(cnt_register.SetRawFromHeader());

    Packet pack(b_data, this->BP_CNT_WRITE_MULTIPLE_AWAIT_SIZE, 100);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_WriteSingleData(CONTROL_IDs id, CONTROL_Registers reg, float data){
    QVector<float>  tmp_data; tmp_data.append(data);
    BP_Header       bp_header(this->BP_CONTROL, 4);
    CNT_Header      cnt_header(id, CNT_WRITE_REG);
    CNT_Register    cnt_register(reg, tmp_data);

    QByteArray b_data = bp_header.SetRawFromHeader();
    b_data.append(cnt_header.SetRawFromHeader());
    b_data.append(cnt_register.SetRawFromHeader());

    Packet pack(b_data, this->BP_CNT_WRITE_SINGLE_AWAIT_SIZE, 100);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_SendCmd(CONTROL_Commands cmd){
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

        }

        emit siSendSerial(this->Serial);

        if (s_name == BOOTLOADER_ID){
            disconnect(this->Serial, &QSerialPort::readyRead, this, &SystemControl::PushDataFromStream);
            if (this->DeviceCheckTimer->isActive()) this->DeviceCheckTimer->stop();
            if (this->TransmitHandlerTimer->isActive()) this->TransmitHandlerTimer->stop();
            emit siChooseTab(BOOTLOADER_TAB);
        }
        else if (s_name == APP_2_ID){
            disconnect(this->Serial, &QSerialPort::readyRead, this, &SystemControl::PushDataFromStream);
            if (this->DeviceCheckTimer->isActive()) this->DeviceCheckTimer->stop();
            if (this->TransmitHandlerTimer->isActive()) this->TransmitHandlerTimer->stop();
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
    case(this->BP_CONTROL):{
        CNT_Header cnt_header;
        cnt_header.SetHeaderFromBPPayload(&bp_header);
        CNT_Register cnt_register;
        cnt_register.SetHeaderFromCNTPayload(&cnt_header);
        switch(cnt_header.cmd){
        case(CNT_WRITE_REG):
            switch(cnt_header.id){
            case(CNT_ID_GLOBAL):
                break;
            default:
                break;
            }
            break;
        case(CNT_READ_REG):
            switch(cnt_header.id){
            case(CNT_ID_GLOBAL):
                if (this->RegisterNames[cnt_register.reg].is_active){
                    QAction * p_act = this->plot_context_menu.findChild<QAction *>("Auto Rescale");
                    for (uint8_t iter = 0; iter < 4; iter++){
                        this->RegisterNames[cnt_register.reg].graph_id[iter]->addData(flag, cnt_register.data[iter]);
                        if (p_act->isChecked()) this->plot_handles[iter]->rescaleAxes();
                        this->plot_handles[iter]->replot();
                    }
                    flag++;
                }

                qDebug() << "New data acquired";
                this->SerialLock.Unlock();
                break;
            default:
                break;
            }
            break;
        case(CNT_BLOCK_DRIVE):
            qDebug() << "Drive Locked";
            break;
        case(CNT_ENABLE_DRIVE):
            qDebug() << "Drive Enabled";
            break;
        }

        qDebug() << "Control request";
        qDebug() << "BP:    CMD " << bp_header.cmd << " W_Size " << bp_header.w_size;
        qDebug() << "CNT:   ID  " << cnt_header.id << " CMD    " << cnt_header.cmd;
        qDebug() << "REG:   " << cnt_register.reg;
        qDebug() << "DATA:  " << cnt_register.data[0] << " " << cnt_register.data[1] << " " << cnt_register.data[2] << " " << cnt_register.data[3];
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
    this->DeviceCheckTimer->start(1000);
    this->TransmitHandlerTimer->start(10);
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
        this->plot_handles[iter]->xAxis->setLabel("Time");
        this->plot_handles[iter]->yAxis->setLabel("Parameter");
        this->plot_handles[iter]->xAxis->setRange(0, 10);
        this->plot_handles[iter]->yAxis->setRange(-1, 1);
    }

    QAction * p_act = nullptr;
    this->plot_context_menu.addAction("Clear", this, &SystemControl::slClearPlots)->setObjectName("Clear");
    this->plot_context_menu.addAction("Rescale", this, &SystemControl::slRescalePlots)->setObjectName("Rescale");
    p_act = this->plot_context_menu.addAction("Auto Rescale");
    p_act->setObjectName("Auto Rescale");
    p_act->setCheckable(true);
    connect(p_act, &QAction::triggered, this, &SystemControl::slAutoRescalePlots);

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

    for (uint8_t iter = 0; iter < 4; iter++){
        this->plot_handles[iter]->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this->plot_handles[iter], &QCustomPlot::customContextMenuRequested, this, &SystemControl::slShowContextMenu);
    }
}

void SystemControl::slClearPlots(void){
    this->C_ReadMultipleData(CNT_REG_CUR_FB);
//    for (uint8_t iter = 0; iter < 4; iter++)
//        this->plot_handles[iter]->clearGraphs();
}

void SystemControl::slShowContextMenu(const QPoint & pos){
    this->plot_context_menu.exec(QCursor::pos());
}

void SystemControl::slRescalePlots(void){

}

void SystemControl::slAutoRescalePlots(bool state){
    QAction * p_act = this->plot_context_menu.findChild<QAction *>("Rescale");
    p_act->setEnabled(!state);
}

void SystemControl::slParameterHandle(bool state){
    QString sender_name = sender()->objectName();
    QMenu * tmp = this->plot_context_menu.findChild<QMenu *>("Parameter Menu");

    if (sender_name == this->RegisterNames[CNT_REG_TORQUE].name){
        this->RegisterNames[CNT_REG_TORQUE].is_active = state;
        this->AttachRegisterToGraph(this->RegisterNames[CNT_REG_TORQUE].graph_id, state);
        for (auto iter = tmp->actions().begin(); iter != tmp->actions().end(); iter++){
            if ((*iter)->objectName() != this->RegisterNames[CNT_REG_TORQUE].name)
                (*iter)->setEnabled(!state);
        }
    }
    else if ((sender_name == this->RegisterNames[CNT_REG_POS_SP].name) ||
        (sender_name == this->RegisterNames[CNT_REG_POS_FB].name) ||
        (sender_name == this->RegisterNames[CNT_REG_POS_ACC].name)){

        QString sp = this->RegisterNames[CNT_REG_POS_SP].name;
        QString fb = this->RegisterNames[CNT_REG_POS_FB].name;
        QString acc = this->RegisterNames[CNT_REG_POS_ACC].name;

            if (sender_name == sp){
                this->RegisterNames[CNT_REG_POS_SP].is_active = state;
                this->AttachRegisterToGraph(this->RegisterNames[CNT_REG_POS_SP].graph_id, state);
            }
            else if (sender_name == fb){
                this->RegisterNames[CNT_REG_POS_FB].is_active = state;
                this->AttachRegisterToGraph(this->RegisterNames[CNT_REG_POS_FB].graph_id, state);
            }
            else if (sender_name == acc){
                this->RegisterNames[CNT_REG_POS_ACC].is_active = state;
                this->AttachRegisterToGraph(this->RegisterNames[CNT_REG_POS_ACC].graph_id, state);
            }

            if ((!(this->RegisterNames[CNT_REG_POS_SP].is_active)) &&
                (!(this->RegisterNames[CNT_REG_POS_FB].is_active)) &&
                (!(this->RegisterNames[CNT_REG_POS_ACC].is_active)))
                    for (auto iter = tmp->actions().begin(); iter != tmp->actions().end(); iter++){
                        if (((*iter)->objectName() != sp) && ((*iter)->objectName() != fb) && ((*iter)->objectName() != acc))
                            (*iter)->setEnabled(true);
                    }
            else{
                for (auto iter = tmp->actions().begin(); iter != tmp->actions().end(); iter++){
                    if (((*iter)->objectName() != sp) && ((*iter)->objectName() != fb) && ((*iter)->objectName() != acc))
                        (*iter)->setEnabled(false);
                }
            }
    }
    else if ((sender_name == this->RegisterNames[CNT_REG_SPD_SP].name) ||
        (sender_name == this->RegisterNames[CNT_REG_SPD_FB].name) ||
        (sender_name == this->RegisterNames[CNT_REG_SPD_ACC].name)){

        QString sp = this->RegisterNames[CNT_REG_SPD_SP].name;
        QString fb = this->RegisterNames[CNT_REG_SPD_FB].name;
        QString acc = this->RegisterNames[CNT_REG_SPD_ACC].name;
            if (sender_name == sp){
                this->RegisterNames[CNT_REG_SPD_SP].is_active = state;
                this->AttachRegisterToGraph(this->RegisterNames[CNT_REG_SPD_SP].graph_id, state);
            }
            else if (sender_name == fb){
                this->RegisterNames[CNT_REG_SPD_FB].is_active = state;
                this->AttachRegisterToGraph(this->RegisterNames[CNT_REG_SPD_FB].graph_id, state);
            }
            else if (sender_name == acc){
                this->RegisterNames[CNT_REG_SPD_ACC].is_active = state;
                this->AttachRegisterToGraph(this->RegisterNames[CNT_REG_POS_ACC].graph_id, state);
            }

            if ((!(this->RegisterNames[CNT_REG_SPD_SP].is_active)) &&
                (!(this->RegisterNames[CNT_REG_SPD_FB].is_active)) &&
                (!(this->RegisterNames[CNT_REG_SPD_ACC].is_active)))
                    for (auto iter = tmp->actions().begin(); iter != tmp->actions().end(); iter++){
                        if (((*iter)->objectName() != sp) && ((*iter)->objectName() != fb) && ((*iter)->objectName() != acc))
                            (*iter)->setEnabled(true);
                    }
            else{
                for (auto iter = tmp->actions().begin(); iter != tmp->actions().end(); iter++){
                    if (((*iter)->objectName() != sp) && ((*iter)->objectName() != fb) && ((*iter)->objectName() != acc))
                        (*iter)->setEnabled(false);
                }
            }
    }
    else if ((sender_name == this->RegisterNames[CNT_REG_CUR_SP].name) ||
        (sender_name == this->RegisterNames[CNT_REG_CUR_FB].name) ||
        (sender_name == this->RegisterNames[CNT_REG_CUR_ACC].name)){

        QString sp = this->RegisterNames[CNT_REG_CUR_SP].name;
        QString fb = this->RegisterNames[CNT_REG_CUR_FB].name;
        QString acc = this->RegisterNames[CNT_REG_CUR_ACC].name;
            if (sender_name == sp){
                this->RegisterNames[CNT_REG_CUR_SP].is_active = state;
                this->AttachRegisterToGraph(this->RegisterNames[CNT_REG_CUR_SP].graph_id, state);
            }
            else if (sender_name == fb){
                this->RegisterNames[CNT_REG_CUR_FB].is_active = state;
                this->AttachRegisterToGraph(this->RegisterNames[CNT_REG_CUR_FB].graph_id, state);
            }
            else if (sender_name == acc){
                this->RegisterNames[CNT_REG_CUR_ACC].is_active = state;
                this->AttachRegisterToGraph(this->RegisterNames[CNT_REG_CUR_ACC].graph_id, state);
            }

            if ((!(this->RegisterNames[CNT_REG_CUR_SP].is_active)) &&
                (!(this->RegisterNames[CNT_REG_CUR_FB].is_active)) &&
                (!(this->RegisterNames[CNT_REG_CUR_ACC].is_active)))
                    for (auto iter = tmp->actions().begin(); iter != tmp->actions().end(); iter++){
                        if (((*iter)->objectName() != sp) && ((*iter)->objectName() != fb) && ((*iter)->objectName() != acc))
                            (*iter)->setEnabled(true);
                    }
            else{
                for (auto iter = tmp->actions().begin(); iter != tmp->actions().end(); iter++){
                    if (((*iter)->objectName() != sp) && ((*iter)->objectName() != fb) && ((*iter)->objectName() != acc))
                        (*iter)->setEnabled(false);
                }
            }
    }
    else if (sender_name == this->RegisterNames[CNT_REG_OUTPUT].name){
        this->RegisterNames[CNT_REG_OUTPUT].is_active = state;
        this->AttachRegisterToGraph(this->RegisterNames[CNT_REG_OUTPUT].graph_id, state);
        for (auto iter = tmp->actions().begin(); iter != tmp->actions().end(); iter++){
            if ((*iter)->objectName() != this->RegisterNames[CNT_REG_OUTPUT].name)
                (*iter)->setEnabled(!state);
        }
    }
}

void SystemControl::AttachRegisterToGraph(QCPGraph ** graph, bool state){
    if (state){
        graph[0] = ui->widget_plot1->addGraph();
        graph[1] = ui->widget_plot2->addGraph();
        graph[2] = ui->widget_plot3->addGraph();
        graph[3] = ui->widget_plot4->addGraph();
    }
    else{
        ui->widget_plot1->removeGraph(graph[0]);
        ui->widget_plot2->removeGraph(graph[1]);
        ui->widget_plot3->removeGraph(graph[2]);
        ui->widget_plot4->removeGraph(graph[3]);
    }
}

