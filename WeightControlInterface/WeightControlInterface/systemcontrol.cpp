#include "systemcontrol.h"
#include "ui_systemcontrol.h"

SystemControl::SystemControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemControl)
{
    ui->setupUi(this);


}

SystemControl::~SystemControl()
{
    delete ui;
}
