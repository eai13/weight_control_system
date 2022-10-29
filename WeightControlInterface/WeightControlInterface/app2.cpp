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
    ui->listWidget_items->addItem("Time");
    ui->listWidget_items->addItem("Threshold");
    ui->listWidget_items->addItem("Upper Threshold");
    ui->listWidget_items->addItem("Lower Threshold");
    ui->listWidget_items->addItem("Deadzone");
    ui->listWidget_items->addItem("Plot");
    ui->listWidget_items->addItem("PID Controller");

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
    case(6):{
        APP2_timeblock * new_block = new APP2_timeblock(ui->graphicsView_canvas->scene());
        connect(this, &APP2::siPrepareForProcessing, new_block, &APP2_timeblock::slPrepareForProcessing);
        connect(new_block, &APP2_timeblock::siBlockRemoved, this, &APP2::slBlockRemoved);
        this->block_list.push_back(new_block);
        break;
    }
    case(7):{
        APP2_thresholdblock * new_block = new APP2_thresholdblock(ui->graphicsView_canvas->scene(), 0, 0);
        connect(this, &APP2::siPrepareForProcessing, new_block, &APP2_thresholdblock::slPrepareForProcessing);
        connect(new_block, &APP2_thresholdblock::siBlockRemoved, this, &APP2::slBlockRemoved);
        this->block_list.push_back(new_block);
        break;
    }
    case(8):{
        APP2_upperthresblock * new_block = new APP2_upperthresblock(ui->graphicsView_canvas->scene(), 0);
        connect(this, &APP2::siPrepareForProcessing, new_block, &APP2_upperthresblock::slPrepareForProcessing);
        connect(new_block, &APP2_upperthresblock::siBlockRemoved, this, &APP2::slBlockRemoved);
        this->block_list.push_back(new_block);
        break;
    }
    case(9):{
        APP2_lowerthresblock * new_block = new APP2_lowerthresblock(ui->graphicsView_canvas->scene(), 0);
        connect(this, &APP2::siPrepareForProcessing, new_block, &APP2_lowerthresblock::slPrepareForProcessing);
        connect(new_block, &APP2_lowerthresblock::siBlockRemoved, this, &APP2::slBlockRemoved);
        this->block_list.push_back(new_block);
        break;
    }
    case(10):{
        APP2_deadzoneblock * new_block = new APP2_deadzoneblock(ui->graphicsView_canvas->scene(), -0.5, 0.5);
        connect(this, &APP2::siPrepareForProcessing, new_block, &APP2_deadzoneblock::slPrepareForProcessing);
        connect(new_block, &APP2_deadzoneblock::siBlockRemoved, this, &APP2::slBlockRemoved);
        this->block_list.push_back(new_block);
        break;
    }
    case(11):{
        APP2_plotblock * new_block = new APP2_plotblock(ui->graphicsView_canvas->scene());
        connect(this, &APP2::siPrepareForProcessing, new_block, &APP2_plotblock::slPrepareForProcessing);
        connect(new_block, &APP2_plotblock::siBlockRemoved, this, &APP2::slBlockRemoved);
        this->block_list.push_back(new_block);
        break;
    }
    case(12):{
        APP2_PIDblock * new_block = new APP2_PIDblock(ui->graphicsView_canvas->scene());
        connect(this, &APP2::siPrepareForProcessing, new_block, &APP2_PIDblock::slPrepareForProcessing);
        connect(new_block, &APP2_PIDblock::siBlockRemoved, this, &APP2::slBlockRemoved);
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
