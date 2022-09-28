#include "app2.h"
#include "ui_app2.h"

APP2::APP2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::APP2)
{
    ui->setupUi(this);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &APP2::customContextMenuRequested, this, &APP2::slContextMenuRequested);
}

APP2::~APP2()
{
    delete ui;
}

void APP2::slContextMenuRequested(const QPoint &pos){
    qDebug() << "ContextMenu from APP2";
    app2_constblock * block;
}
