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

//    this->canvas_menu.clear();
//    ui->graphicsView_canvas->setContextMenuPolicy(Qt::CustomContextMenu);
//    this->canvas_menu.addAction("Save Model");
//    this->canvas_menu.addAction("Upload Model");

//    this->canvas_menu.addSeparator();

//        QMenu * block_menu = this->canvas_menu.addMenu("Add Block");
//        block_menu->addAction("Constant", this, &APP2::slAddConstantBlock);
//        block_menu->addAction("Sum", this, &APP2::slAddSumBlock);
//        block_menu->addAction("Product", this, &APP2::slAddMultiplyBlock);
//        block_menu->addAction("Division", this, &APP2::slAddDivBlock);

//    connect(ui->graphicsView_canvas, &QGraphicsView::customContextMenuRequested, this, &APP2::slContextMenuRequested);

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
