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
}

MainWindow::~MainWindow()
{
    delete ui;
}

