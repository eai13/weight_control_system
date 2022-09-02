#include "systemcontrol.h"
#include "ui_systemcontrol.h"

SystemControl::SystemControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemControl)
{
    ui->setupUi(this);


}

void SystemControl::C_PingSilent(void){
    if (!(this->SerialLock.Lock())){
        return;
    }
    qDebug() << "Ping Silent";
    BP_Header header(this->BP_PING, 0);

    this->data_awaited = this->BP_PING_AWAIT_SIZE;
    this->Serial->write(header.SetRawFromHeader());
    this->TimeoutTimer->start(50);
}

SystemControl::~SystemControl()
{
    delete ui;
}

void SystemControl::ProcessIncomingData(void){
    if ((this->data_awaited) && (this->Serial->bytesAvailable() >= this->data_awaited))
        this->ProcessIncomingData();
}

void SystemControl::PushDataFromStream(void){

}

void SystemControl::slActivate(void){
    connect(this->Serial, &QSerialPort::readyRead, this, &SystemControl::PushDataFromStream);

}
