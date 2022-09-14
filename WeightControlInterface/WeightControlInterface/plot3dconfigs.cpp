#include "plot3dconfigs.h"
#include "ui_plot3dconfigs.h"

Plot3DConfigs::Plot3DConfigs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Plot3DConfigs)
{
    ui->setupUi(this);
}

Plot3DConfigs::~Plot3DConfigs()
{
    delete ui;
}


void Plot3DConfigs::slUpdateBar(int perc){

}
