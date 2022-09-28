#include "app2_constblock.h"
#include "ui_app2_constblock.h"

app2_constblock::app2_constblock(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::app2_constblock)
{
    ui->setupUi(this);
}

app2_constblock::~app2_constblock()
{
    delete ui;
}
