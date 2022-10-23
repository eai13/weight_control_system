#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bootloader.h"
#include "systemcontrol.h"
#include "app2.h"

Bootloader *    bootloader_ui;
SystemControl * systemcontrol_ui;
APP2 *          app2;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bootloader_ui = new Bootloader();
    ui->tabWidget_main->addTab(bootloader_ui, "Bootloader");
    systemcontrol_ui = new SystemControl();
    ui->tabWidget_main->addTab(systemcontrol_ui, "Control System");
    app2 = new APP2();
    ui->tabWidget_main->addTab(app2, "EBALA");

    connect(this, &MainWindow::siActivateBOOT, bootloader_ui, &Bootloader::slActivate);
    connect(this, &MainWindow::siActivateAPP1, systemcontrol_ui, &SystemControl::slActivate);

    connect(bootloader_ui, &Bootloader::siChooseTab, this, &MainWindow::slChooseTab);
    connect(systemcontrol_ui, &SystemControl::siChooseTab, this, &MainWindow::slChooseTab);

    connect(bootloader_ui, &Bootloader::siSendSerial, systemcontrol_ui, &SystemControl::slReceiveSerial);
    connect(systemcontrol_ui, &SystemControl::siSendSerial, bootloader_ui, &Bootloader::slReceiveSerial);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slChooseTab(uint16_t tab){
    if (tab >= ui->tabWidget_main->count()){
        return;
    }

    ui->tabWidget_main->setCurrentIndex(tab);

    for (int iter = 0; iter < ui->tabWidget_main->count(); iter++){
        if (iter != tab)
            ui->tabWidget_main->setTabEnabled(iter, false);
        else
            ui->tabWidget_main->setTabEnabled(iter, true);
    }

    switch(tab){
    case(0):
        emit this->siActivateBOOT();
        break;
    case(1):
        emit this->siActivateAPP1();
        break;
    case(2):
        emit this->siActivateAPP2();
        break;
    default:
        break;
    }
}
