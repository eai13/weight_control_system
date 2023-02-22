#include "systemcontrol.h"
#include "ui_systemcontrol.h"
#include "qcustomplot.h"
#include "plot3dconfigs.h"
#include <QVector>
#include <QMenu>
#include <QCursor>
#include <QAction>
#include <QActionGroup>

#include <iostream>

SystemControl::SystemControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemControl)
{
    ui->setupUi(this);

    this->plot3d = new Plot3D(ui->groupBox_plot3d);

    Plot3DConfigs * plot3dconfigs = new Plot3DConfigs();
    ui->groupBox_3dplot_settings->layout()->addWidget(plot3dconfigs);
    connect(plot3dconfigs,  &Plot3DConfigs::siFullscreen,           this->plot3d,   &Plot3D::slFullscreen);
    connect(plot3dconfigs,  &Plot3DConfigs::siTargetAdd,            this->plot3d,   &Plot3D::slTargetAdd);
    connect(plot3dconfigs,  &Plot3DConfigs::siTargetClear,          this->plot3d,   &Plot3D::slTargetClear);
    connect(plot3dconfigs,  &Plot3DConfigs::siTargetRemove,         this->plot3d,   &Plot3D::slTargetRemove);
    connect(plot3dconfigs,  &Plot3DConfigs::siStartTrajectory,      this->plot3d,   &Plot3D::slStartTrajectory);
    connect(plot3dconfigs,  &Plot3DConfigs::siStopTrajectory,       this->plot3d,   &Plot3D::slAbortTrajectory);
    connect(plot3dconfigs,  &Plot3DConfigs::siPauseTrajectory,      this->plot3d,   &Plot3D::slPauseTrajectory);
    connect(plot3dconfigs,  &Plot3DConfigs::siSaveTarget,           this->plot3d,   &Plot3D::slSaveTarget);
    connect(plot3dconfigs,  &Plot3DConfigs::siSaveReal,             this->plot3d,   &Plot3D::slSaveReal);
    connect(plot3dconfigs,  &Plot3DConfigs::siUploadTarget,         this->plot3d,   &Plot3D::slUploadTarget);
    connect(plot3dconfigs,  &Plot3DConfigs::siSendObjectStep,       this->plot3d,   &Plot3D::slReceiveObjectStep);
    connect(this->plot3d,   &Plot3D::siEnableAbort,                 plot3dconfigs,  &Plot3DConfigs::slEnableAbort);
    connect(this->plot3d,   &Plot3D::siEnableStop,                  plot3dconfigs,  &Plot3DConfigs::slEnableStop);
    connect(this->plot3d,   &Plot3D::siEnableStart,                 plot3dconfigs,  &Plot3DConfigs::slEnableStart);
    connect(this->plot3d,   &Plot3D::siEnableFullscreen,            plot3dconfigs,  &Plot3DConfigs::slEnableFullscreen);
    connect(this->plot3d,   &Plot3D::siSendTargetLength,            this,           &SystemControl::slSendLength);
    connect(this,           &SystemControl::siSendRealLength,       this->plot3d,   &Plot3D::slReceiveRealLength);
    connect(this,           &SystemControl::siSendSetpointLength,   this->plot3d,   &Plot3D::slReceiveSetpointLength);

    this->SystemTime = new QTime();
    this->SystemTime->start();

    this->plots[0] = new Plot2D("Motor 1", ui->widget_plot1,
                                ui->radioButton_m1turns, ui->radioButton_m1rads, ui->radioButton_m1length,
                                ui->dial_motor1,
                                ui->lineEdit_motor1pos,
                                ui->pushButton_motor1zerocalib, ui->pushButton_motor1stop, ui->pushButton_motor1setpos,
                                ui->pushButton_motor1jogminusSMALL, ui->pushButton_motor1jogminusBIG,
                                ui->pushButton_motor1jogplusSMALL, ui->pushButton_motor1jogplusBIG,
                                MOTOR_1_A_CALIB, MOTOR_1_B_CALIB, MOTOR_1_C_CALIB, MOTOR_1_MIN, MOTOR_1_MAX);
    this->plots[1] = new Plot2D("Motor 2", ui->widget_plot2,
                                ui->radioButton_m2turns, ui->radioButton_m2rads, ui->radioButton_m2length,
                                ui->dial_motor2,
                                ui->lineEdit_motor2pos,
                                ui->pushButton_motor2zerocalib, ui->pushButton_motor2stop, ui->pushButton_motor2setpos,
                                ui->pushButton_motor2jogminusSMALL, ui->pushButton_motor2jogminusBIG,
                                ui->pushButton_motor2jogplusSMALL, ui->pushButton_motor2jogplusBIG,
                                MOTOR_2_A_CALIB, MOTOR_2_B_CALIB, MOTOR_2_C_CALIB, MOTOR_2_MIN, MOTOR_2_MAX);
    this->plots[2] = new Plot2D("Motor 3", ui->widget_plot3,
                                ui->radioButton_m3turns, ui->radioButton_m3rads, ui->radioButton_m3length,
                                ui->dial_motor3,
                                ui->lineEdit_motor3pos,
                                ui->pushButton_motor3zerocalib, ui->pushButton_motor3stop, ui->pushButton_motor3setpos,
                                ui->pushButton_motor3jogminusSMALL, ui->pushButton_motor3jogminusBIG,
                                ui->pushButton_motor3jogplusSMALL, ui->pushButton_motor3jogplusBIG,
                                MOTOR_3_A_CALIB, MOTOR_3_B_CALIB, MOTOR_3_C_CALIB, MOTOR_3_MIN, MOTOR_3_MAX);
    this->plots[3] = new Plot2D("Motor 4", ui->widget_plot4,
                                ui->radioButton_m4turns, ui->radioButton_m4rads, ui->radioButton_m4length,
                                ui->dial_motor4,
                                ui->lineEdit_motor4pos,
                                ui->pushButton_motor4zerocalib, ui->pushButton_motor4stop, ui->pushButton_motor4setpos,
                                ui->pushButton_motor4jogminusSMALL, ui->pushButton_motor4jogminusBIG,
                                ui->pushButton_motor4jogplusSMALL, ui->pushButton_motor4jogplusBIG,
                                MOTOR_4_A_CALIB, MOTOR_4_B_CALIB, MOTOR_4_C_CALIB, MOTOR_4_MIN, MOTOR_4_MAX);

    for (uint8_t iter = 0; iter < 4; iter++){
        this->plots[iter]->setObjectName(QString::fromStdString(std::to_string(iter)));
        connect(this->plots[iter],  &Plot2D::siSendPos,                 this,               &SystemControl::slSendPos);
        connect(this->plots[iter],  &Plot2D::siCalibrateZero,           this,               &SystemControl::slSendZeroCalibration);
        connect(this->plots[iter],  &Plot2D::siStopDrive,               this,               &SystemControl::slStopDrive);
        connect(this->plots[iter],  &Plot2D::siSendSetPos,              this,               &SystemControl::slReceiveSetPos);
        connect(plot3dconfigs,      &Plot3DConfigs::siStartTrajectory,  this->plots[iter],  &Plot2D::slBlockModule);
        connect(plot3dconfigs,      &Plot3DConfigs::siPauseTrajectory,  this->plots[iter],  &Plot2D::slBlockModule);
        connect(plot3dconfigs,      &Plot3DConfigs::siStopTrajectory,   this->plots[iter],  &Plot2D::slEnableModule);
    }
    connect(this, &SystemControl::siSendMotor1Rads, this->plots[0], &Plot2D::slReceiveActualPosition);
    connect(this, &SystemControl::siSendMotor2Rads, this->plots[1], &Plot2D::slReceiveActualPosition);
    connect(this, &SystemControl::siSendMotor3Rads, this->plots[2], &Plot2D::slReceiveActualPosition);
    connect(this, &SystemControl::siSendMotor4Rads, this->plots[3], &Plot2D::slReceiveActualPosition);

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
    qDebug() << "System Control PING";
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

void SystemControl::C_SendSingleDriveCmd(uint16_t cmd, uint16_t id){
    BP_Header       bp_header(this->BP_CONTROL, 2);
    CNT_Header      cnt_header(id, cmd);

    QByteArray b_data = bp_header.SetRawFromHeader();
    b_data.append(cnt_header.SetRawFromHeader());

    Packet pack(b_data, this->BP_CNT_SIGNLE_DRIVE_CMD_AWAIT_SIZE, 100);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_SendGlobalCmd(uint16_t cmd){
    BP_Header       bp_header(this->BP_CONTROL ,2);
    CNT_Header      cnt_header(CNT_ID_GLOBAL, cmd);

    QByteArray b_data = bp_header.SetRawFromHeader();
    b_data.append(cnt_header.SetRawFromHeader());

    Packet pack(b_data, this->BP_CNT_GLOBAL_CMD_AWAIT_SIZE, 100);
    this->serial_tx_queue.push_back(pack);
}

void SystemControl::C_SendCalibrateEncoder(uint16_t id, float value){
    BP_Header       bp_header(this->BP_CONTROL, 3);
    CNT_Header      cnt_header(id, CNT_CALIBRATE_ENCODER);

    QByteArray b_data = bp_header.SetRawFromHeader();
    b_data.append(cnt_header.SetRawFromHeader());
    b_data.append(reinterpret_cast<char *>(&value), sizeof(float));
    qDebug() << "DATA AMOUNT TO SEND: " << b_data.size();

    Packet pack(b_data, this->BP_CNT_CALIBRATE_ENCODER_AWAIT_SIZE, 100);
    this->serial_tx_queue.push_back(pack);
}

SystemControl::~SystemControl()
{
    delete ui;
}

void SystemControl::ProcessIncomingData(void){

    QByteArray data = this->Serial->read(this->data_awaited);
    this->data_awaited = 0;
    this->Serial->readAll();

    BP_Header bp_header;
    if (bp_header.SetHeaderFromRaw(data) == -1){
        this->SerialLock.Unlock();
        return;
    }

    if (this->TimeoutTimer->isActive())
        this->TimeoutTimer->stop();

    if ((bp_header.start_5A != 0x5A) || (bp_header.start_A5 != 0xA5)){
        this->SerialLock.Unlock();
        return;
    }

    switch(bp_header.cmd){
    case(BP_Commands::BP_PING):{
        uint32_t id = bp_header.payload[0];
        char * name = reinterpret_cast<char *>(&id);
        QString s_name = "";
        s_name += name[0]; s_name += name[1]; s_name += name[2]; s_name += name[3];
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
    case(BP_Commands::BP_JUMP):{
        ConsoleBasic("Jump back to boot");
        this->PlottableDataTimer->stop();
        this->serial_tx_queue.clear();
        this->SerialLock.Unlock();
        break;
    }
    case(BP_Commands::BP_CONTROL):{
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
//                qDebug() << cnt_header.cmd << " " << cnt_header.id;
//                qDebug() << cnt_register.reg << " " << cnt_register.data;
                break;
            }
            break;
        }
        case(CNT_READ_REG):{
            CNT_Register cnt_register;
            cnt_register.SetHeaderFromCNTPayload(&cnt_header);
            switch(cnt_header.id){
            case(CNT_ID_GLOBAL):{
                this->SerialLock.Unlock();
                switch(cnt_register.reg){
                case(CNT_REG_POS_SP):{
                    for (uint8_t iter = 0; iter < 4; iter++){
                        if (this->plots[iter]->rb_rads->isChecked()){
                            this->plots[iter]->lineedit->setText(QString::asprintf("%.2f", cnt_register.data[iter]));
                        }
                        else{
                            this->plots[iter]->lineedit->setText(QString::asprintf("%.2f", cnt_register.data[iter] / (2*PI)));
                        }
                        this->plots[iter]->slProcessEditLine();
                    }
                    break;
                }
                }
                break;
            }
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
            emit this->siSendMotor1Rads(cnt_plottable.data[1], cnt_plottable.data[0]);
            emit this->siSendMotor2Rads(cnt_plottable.data[8], cnt_plottable.data[7]);
            emit this->siSendMotor3Rads(cnt_plottable.data[15], cnt_plottable.data[14]);
            emit this->siSendMotor4Rads(cnt_plottable.data[22], cnt_plottable.data[21]);
            emit this->siSendRealLength(
                    this->plots[0]->GetLengthFromAngle(cnt_plottable.data[1]),
                    this->plots[1]->GetLengthFromAngle(cnt_plottable.data[8]),
                    this->plots[2]->GetLengthFromAngle(cnt_plottable.data[15]),
                    this->plots[3]->GetLengthFromAngle(cnt_plottable.data[22]));
            emit this->siSendSetpointLength(
                    this->plots[0]->GetLengthFromAngle(cnt_plottable.data[0]),
                    this->plots[1]->GetLengthFromAngle(cnt_plottable.data[7]),
                    this->plots[2]->GetLengthFromAngle(cnt_plottable.data[14]),
                    this->plots[3]->GetLengthFromAngle(cnt_plottable.data[21]));
            break;
        }
        case(CNT_CALIBRATE_ZERO):{
            switch(cnt_header.id){
            case(CNT_ID_GLOBAL):{
                qDebug() << "All Motors Zero-Calibrated";
                break;
            }
            default:{
                qDebug() << "Single Motor Zero-Calibrated " << cnt_header.id;
                break;
            }
            }

            qDebug() << "Motor Calibrated to Zero";
            break;
        }
        case(CNT_CALIBRATE_ENCODER):{
            qDebug() << "ENCODER CALIBRATED";
            break;
        }
        case(CNT_STOP_DRIVE):{
            qDebug() << "Drive Stopped " << cnt_header.id;
            break;
        }
        }

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
    this->DeviceCheckTimer->start(TIMER_PERIODS::PERIODDeviceCheckTimer);
    this->PlottableDataTimer->start(TIMER_PERIODS::PERIODPlotDataTimer);
    this->TransmitHandlerTimer->start(TIMER_PERIODS::PERIODTransmitHandlerTimer);

    this->C_ReadMultipleData(CONTROL_Registers::CNT_REG_POS_SP);
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

void SystemControl::ConsoleDebug(QString message){
    ui->listWidget_debugconsole->addItem("[DEBUG] " + message);
    ui->listWidget_debugconsole->item(ui->listWidget_debugconsole->count() - 1)->setForeground(QColor(0, 0, 255));
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
    this->C_WriteSingleData(sender_id, CONTROL_Registers::CNT_REG_POS_SP, data);
}

void SystemControl::slSendZeroCalibration(void){
    uint8_t sender_id = sender()->objectName().toInt();
    this->C_SendSingleDriveCmd(CONTROL_Commands::CNT_CALIBRATE_ZERO, sender_id);
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

void SystemControl::slSendLength(float len1, float len2, float len3, float len4){
    QVector<float> rads;
    rads.push_back(this->plots[0]->GetAngleFromLength(len1));
    rads.push_back(this->plots[1]->GetAngleFromLength(len2));
    rads.push_back(this->plots[2]->GetAngleFromLength(len3));
    rads.push_back(this->plots[3]->GetAngleFromLength(len4));

    this->C_WriteMultipleData(CONTROL_Registers::CNT_REG_POS_SP, rads);
}

void SystemControl::slStopDrive(void){
    uint8_t sender_id = sender()->objectName().toInt();
    this->C_SendSingleDriveCmd(CONTROL_Commands::CNT_STOP_DRIVE, sender_id);
}
void SystemControl::slStopDriveGlobal(void){
    this->C_SendGlobalCmd(CONTROL_Commands::CNT_STOP_DRIVE);
}

void SystemControl::slReceiveSetPos(float length){
    uint8_t sender_id = sender()->objectName().toInt();
    float radians = this->plots[sender_id]->GetAngleFromLength(length);

    this->C_SendCalibrateEncoder(sender_id, radians);
}
