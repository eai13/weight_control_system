#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "bootloader.h"

Bootloader * bootloader_ui;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    bootloader_ui = new Bootloader();
    ui->tabWidget_main->addTab(bootloader_ui, "Bootloader");
}

MainWindow::~MainWindow()
{
    delete ui;
}

