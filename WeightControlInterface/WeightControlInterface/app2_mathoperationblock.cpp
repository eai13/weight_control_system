#include "app2_mathoperationblock.h"

APP2_mathoperationblock::APP2_mathoperationblock(QGraphicsScene * parent, BlockType bt){
    this->block_type = bt;

    this->output = new APP2_signalnode();
    this->signalnodes.push_back(this->output);

    this->input_1 = new APP2_slotnode();
    this->input_2 = new APP2_slotnode();
    this->slotnodes.push_back(this->input_1);
    this->slotnodes.push_back(this->input_2);

    this->mommy_canvas = parent;
    for (auto iter = this->signalnodes.begin(); iter != this->signalnodes.end(); iter++)
        this->mommy_canvas->addItem(*iter);
    for (auto iter = this->slotnodes.begin(); iter != this->slotnodes.end(); iter++)
        this->mommy_canvas->addItem(*iter);

    switch(bt){
    case(BlockType::MATH_BLOCK_PRODUCT):{
        this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Product")));
        break;
    }
    case(BlockType::MATH_BLOCK_DIF):{
        this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Subtract")));
        break;
    }
    case(BlockType::MATH_BLOCK_POW):{
        this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("To Power")));
        break;
    }
    case(BlockType::MATH_BLOCK_DIV):{
        this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Division")));
        break;
    }
    case(BlockType::MATH_BLOCK_SUM):{
        this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Sum")));
        break;
    }
    }

    this->mommy_canvas->addItem(this->resizenode = new APP2_resizenode);

    connect(this->simpleblock, &APP2_simpleblock::siMoved, this->resizenode, &APP2_resizenode::slMoveTo);
    connect(this->resizenode, &APP2_resizenode::siMoved, this->simpleblock, &APP2_simpleblock::slResize);
}
