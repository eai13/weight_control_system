#include "systemcontrol.h"
#include "ui_systemcontrol.h"
#include "qcustomplot.h"
#include <QVector>
#include <QMenu>

SystemControl::SystemControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemControl)
{
    ui->setupUi(this);

//    ui->widget_plot1->setContextMenuPolicy(Qt::CustomContextMenu);
//    ui->pushButton->setContextMenuPolicy(Qt::CustomContextMenu);
//    connect(ui->widget_plot1, &QCustomPlot::customContextMenuRequested, this, &SystemControl::ShowContextMenu);

    this->TimeoutTimer = new QTimer(this);
    connect(this->TimeoutTimer, &QTimer::timeout, this, &SystemControl::Timeout);

    this->DeviceCheckTimer = new QTimer(this);
    connect(this->DeviceCheckTimer, &QTimer::timeout, this, &SystemControl::C_PingSilent);


    connect(ui->pushButton_quitapp, &QPushButton::released, this, &SystemControl::C_Quit);

    QVector<double> x(101), y(101); // initialize with entries 0..100
    for (int i=0; i<101; ++i)
    {
      x[i] = i/50.0 - 1; // x goes from -1 to 1
      y[i] = x[i]*x[i]; // let's plot a quadratic function
    }

//    connect(ui->widget_plot1, )
    // create graph and assign data to it:
//    ui->widget_plot1->addGraph();
    ui->widget_plot1->addGraph();
    ui->widget_plot1->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->widget_plot1->yAxis->setLabel("y");
    ui->widget_plot1->xAxis->setLabel("x");
    // set axes ranges, so we see all data:
    ui->widget_plot1->xAxis->setRange(-1, 1);
    ui->widget_plot1->yAxis->setRange(0, 1);
    ui->widget_plot1->replot();
    ui->widget_plot1->graph(0)->addData(0, 1);

    ui->widget_plot2->addGraph();
    ui->widget_plot2->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->widget_plot2->yAxis->setLabel("y");
    ui->widget_plot2->xAxis->setLabel("x");
    // set axes ranges, so we see all data:
    ui->widget_plot2->xAxis->setRange(-1, 1);
    ui->widget_plot2->yAxis->setRange(0, 1);
    ui->widget_plot2->replot();

    ui->widget_plot3->addGraph();
    ui->widget_plot3->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->widget_plot3->yAxis->setLabel("y");
    ui->widget_plot3->xAxis->setLabel("x");
    // set axes ranges, so we see all data:
    ui->widget_plot3->xAxis->setRange(-1, 1);
    ui->widget_plot3->yAxis->setRange(0, 1);
    ui->widget_plot3->replot();

    ui->widget_plot4->addGraph();
    ui->widget_plot4->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->widget_plot4->yAxis->setLabel("y");
    ui->widget_plot4->xAxis->setLabel("x");
    // set axes ranges, so we see all data:
    ui->widget_plot4->xAxis->setRange(-1, 1);
    ui->widget_plot4->yAxis->setRange(0, 1);
    ui->widget_plot4->replot();
}

void SystemControl::C_PingSilent(void){
    if (!(this->SerialLock.Lock())){
        return;
    }
    qDebug() << "Ping Silent APP1";
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
        qDebug() << "ping receive app1";
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
