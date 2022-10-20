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

    connect(ui->pushButton, &QPushButton::released, this, &APP2::delslCompute);
}

APP2::~APP2()
{
    delete ui;
}

void APP2::slAddItem(const QModelIndex & index){
    switch(index.row()){
    case(0):{
        APP2_constantblock * new_block = new APP2_constantblock(ui->graphicsView_canvas->scene(), 1);
        connect(this, &APP2::siPrepareForProcessing, new_block, &APP2_constantblock::slPrepareForProcessing);
        connect(new_block, &APP2_constantblock::siBlockRemoved, this, &APP2::slBlockRemoved);
        this->block_list.push_back(new_block);
        break;
    }
    case(1):{
        APP2_mathoperationblock * new_block = new APP2_mathoperationblock(ui->graphicsView_canvas->scene(), APP2_mathoperationblock::MATH_BLOCK_PRODUCT);
        connect(this, &APP2::siPrepareForProcessing, new_block, &APP2_mathoperationblock::slPrepareForProcessing);
        connect(new_block, &APP2_mathoperationblock::siBlockRemoved, this, &APP2::slBlockRemoved);
        this->block_list.push_back(new_block);
        break;
    }
    case(2):{
        APP2_mathoperationblock * new_block = new APP2_mathoperationblock(ui->graphicsView_canvas->scene(), APP2_mathoperationblock::MATH_BLOCK_SUM);
        connect(this, &APP2::siPrepareForProcessing, new_block, &APP2_mathoperationblock::slPrepareForProcessing);
        connect(new_block, &APP2_mathoperationblock::siBlockRemoved, this, &APP2::slBlockRemoved);
        this->block_list.push_back(new_block);
        break;
    }
    case(3):{
        APP2_mathoperationblock * new_block = new APP2_mathoperationblock(ui->graphicsView_canvas->scene(), APP2_mathoperationblock::MATH_BLOCK_DIF);
        connect(this, &APP2::siPrepareForProcessing, new_block, &APP2_mathoperationblock::slPrepareForProcessing);
        connect(new_block, &APP2_mathoperationblock::siBlockRemoved, this, &APP2::slBlockRemoved);
        this->block_list.push_back(new_block);
        break;
    }
    case(4):{
        APP2_mathoperationblock * new_block = new APP2_mathoperationblock(ui->graphicsView_canvas->scene(), APP2_mathoperationblock::MATH_BLOCK_DIV);
        connect(this, &APP2::siPrepareForProcessing, new_block, &APP2_mathoperationblock::slPrepareForProcessing);
        connect(new_block, &APP2_mathoperationblock::siBlockRemoved, this, &APP2::slBlockRemoved);
        this->block_list.push_back(new_block);
        break;
    }
    case(5):{
        APP2_mathoperationblock * new_block = new APP2_mathoperationblock(ui->graphicsView_canvas->scene(), APP2_mathoperationblock::MATH_BLOCK_POW);
        connect(this, &APP2::siPrepareForProcessing, new_block, &APP2_mathoperationblock::slPrepareForProcessing);
        connect(new_block, &APP2_mathoperationblock::siBlockRemoved, this, &APP2::slBlockRemoved);
        this->block_list.push_back(new_block);
        break;
    }
    }
}

void APP2::slBlockRemoved(APP2_customblock * block){
    this->block_list.removeOne(block);
}

void APP2::delslCompute(void){
    emit this->siPrepareForProcessing();
    for (auto iter = this->block_list.begin(); iter != this->block_list.end(); iter++){
        (*iter)->ProcessBlockData();
    }
}
