#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bootloader.h"
#include "systemcontrol.h"

Bootloader *    bootloader_ui;
SystemControl * systemcontrol_ui;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bootloader_ui = new Bootloader();
    ui->tabWidget_main->addTab(bootloader_ui, "Bootloader");
    systemcontrol_ui = new SystemControl();
    ui->tabWidget_main->addTab(systemcontrol_ui, "Control System");

    connect(bootloader_ui, &Bootloader::siChooseTab, this, &MainWindow::slChooseTab);

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
    }
}
