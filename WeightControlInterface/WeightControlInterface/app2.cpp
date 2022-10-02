#include "app2.h"
#include "ui_app2.h"

APP2::APP2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::APP2)
{
    ui->setupUi(this);

    ui->listWidget_items->addItem("Constant");
    ui->listWidget_items->addItem("Product");
    ui->listWidget_items->addItem("Sum");
    ui->listWidget_items->addItem("Subtract");
    connect(ui->listWidget_items, &QListWidget::doubleClicked, this, &APP2::slAddItem);

    QGraphicsScene * scene = new QGraphicsScene;
    ui->graphicsView_canvas->setScene(scene);
}

APP2::~APP2()
{
    delete ui;
}

void APP2::slAddItem(const QModelIndex & index){
    switch(index.row()){
    case(0):{
        this->block_list.push_back(new APP2_customblock(ui->graphicsView_canvas->scene()));
        break;
    }
    case(1):{
        qDebug() << "Product Item Added";
        break;
    }
    case(2):{
        qDebug() << "Sum Item Added";
        break;
    }
    case(3):{
        qDebug() << "Subtract Item Added";
        break;
    }
    }
}
