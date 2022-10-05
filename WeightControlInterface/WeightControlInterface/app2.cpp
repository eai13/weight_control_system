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
    ui->listWidget_items->addItem("Subtraction");
    ui->listWidget_items->addItem("Division");
    ui->listWidget_items->addItem("To Power");
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
        this->block_list.push_back(new APP2_constantblock(ui->graphicsView_canvas->scene(), 1));
        break;
    }
    case(1):{
        this->block_list.push_back(new APP2_mathoperationblock(ui->graphicsView_canvas->scene(), APP2_mathoperationblock::MATH_BLOCK_PRODUCT));
        break;
    }
    case(2):{
        this->block_list.push_back(new APP2_mathoperationblock(ui->graphicsView_canvas->scene(), APP2_mathoperationblock::MATH_BLOCK_SUM));
        break;
    }
    case(3):{
        this->block_list.push_back(new APP2_mathoperationblock(ui->graphicsView_canvas->scene(), APP2_mathoperationblock::MATH_BLOCK_DIF));
        break;
    }
    case(4):{
        this->block_list.push_back(new APP2_mathoperationblock(ui->graphicsView_canvas->scene(), APP2_mathoperationblock::MATH_BLOCK_DIV));
        break;
    }
    case(5):{
        this->block_list.push_back(new APP2_mathoperationblock(ui->graphicsView_canvas->scene(), APP2_mathoperationblock::MATH_BLOCK_POW));
        break;
    }
    }
}
