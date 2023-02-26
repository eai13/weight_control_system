#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mathinterpreter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pb_ClearCommandHistory, &QPushButton::released, this, &MainWindow::slClearCommandHistory);
    connect(ui->pb_ClearVariables,      &QPushButton::released, this, &MainWindow::slClearVariables);
    connect(ui->pb_Interpret,           &QPushButton::released, this, &MainWindow::slInterpret);

    MathInterpreter interpreter;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slLogWarning(QString log){
    if (!(ui->cb_Verbose->isChecked())) return;
    ui->list_Log->addItem(log);
    ui->list_Log->item(ui->list_Log->count() - 1)->setTextColor(Qt::yellow);
    ui->list_Log->scrollToBottom();
}

void MainWindow::slLogError(QString log){
    ui->list_Log->addItem(log);
    ui->list_Log->item(ui->list_Log->count() - 1)->setTextColor(Qt::red);
    ui->list_Log->scrollToBottom();
}

void MainWindow::slLogDebug(QString log){
    ui->list_Log->addItem(log);
    ui->list_Log->scrollToBottom();
}

void MainWindow::slClearCommandHistory(void){
    this->slLogDebug("DEBUG");
}

void MainWindow::slClearVariables(void){
    this->slLogError("ERROR");
}

void MainWindow::slInterpret(void){
    this->slLogWarning("WARNING");
}
