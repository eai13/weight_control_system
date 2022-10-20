#include "app2_mathoperationblock.h"

APP2_mathoperationblock::APP2_mathoperationblock(QGraphicsScene * parent, BlockType bt){
    this->block_type = bt;

    this->output = new APP2_signalnode("Output");
    this->signalnodes.push_back(this->output);

    this->input_1 = new APP2_slotnode();
    this->input_2 = new APP2_slotnode();
    switch(bt){
    case(BlockType::MATH_BLOCK_PRODUCT):{
        this->input_1->SetTip("Multiplier 1");
        this->input_2->SetTip("Multiplier 2");
        break;
    }
    case(BlockType::MATH_BLOCK_DIF):{
        this->input_1->SetTip("Minuend");
        this->input_2->SetTip("Subtrahend");
        break;
    }
    case(BlockType::MATH_BLOCK_POW):{
        this->input_1->SetTip("Base");
        this->input_2->SetTip("Power");
        break;
    }
    case(BlockType::MATH_BLOCK_DIV):{
        this->input_1->SetTip("Dividend");
        this->input_2->SetTip("Divisor");
        break;
    }
    case(BlockType::MATH_BLOCK_SUM):{
        this->input_1->SetTip("Term 1");
        this->input_2->SetTip("Term 2");
        break;
    }
    }

    this->slotnodes.push_back(this->input_1);
    this->slotnodes.push_back(this->input_2);

    QList<APP2_valuelabel *> vllist;

    this->mommy_canvas = parent;
    for (auto iter = this->signalnodes.begin(); iter != this->signalnodes.end(); iter++)
        this->mommy_canvas->addItem(*iter);
    for (auto iter = this->slotnodes.begin(); iter != this->slotnodes.end(); iter++)
        this->mommy_canvas->addItem(*iter);

    switch(bt){
    case(BlockType::MATH_BLOCK_PRODUCT):{
        this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(this, QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Product"), vllist));
        this->simpleblock->SetTip("Product");
        break;
    }
    case(BlockType::MATH_BLOCK_DIF):{
        this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(this, QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Subtract"), vllist));
        this->simpleblock->SetTip("Subtract");
        break;
    }
    case(BlockType::MATH_BLOCK_POW):{
        this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(this, QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("To Power"), vllist));
        this->simpleblock->SetTip("To Power");
        break;
    }
    case(BlockType::MATH_BLOCK_DIV):{
        this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(this, QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Division"), vllist));
        this->simpleblock->SetTip("Division");
        break;
    }
    case(BlockType::MATH_BLOCK_SUM):{
        this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(this, QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Sum"), vllist));
        this->simpleblock->SetTip("Sum");
        break;
    }
    }

    this->mommy_canvas->addItem(this->resizenode = new APP2_resizenode);

    connect(this->simpleblock, &APP2_simpleblock::siMainBlockDeleted, this->resizenode, &APP2_resizenode::slMainBlockDeleted);
    connect(this->simpleblock, &APP2_simpleblock::siMoved, this->resizenode, &APP2_resizenode::slMoveTo);
    connect(this->resizenode, &APP2_resizenode::siMoved, this->simpleblock, &APP2_simpleblock::slResize);
}

void APP2_mathoperationblock::ProcessBlockData(void){
    for (auto iter = this->slotnodes.begin(); iter != this->slotnodes.end(); iter++){
        (*iter)->previous_value = (*iter)->current_value;
        (*iter)->current_value = (*iter)->connect_line->sig->previous_value;
    }
//    if (this->simpleblock->is_processed) return;
//    for (auto iter = this->slotnodes.begin(); iter != this->slotnodes.end(); iter++){
//        if ((*iter)->is_processed == false){
//            if ((*iter)->connect_line->sig->base_block == (*iter)->base_block){
//                (*iter)->previous_value = (*iter)->current_value;
//                (*iter)->current_value = (*iter)->connect_line->sig->signal_value;
//                (*iter)->is_processed = true;
//            }
//            else{
//                (*iter)->connect_line->sig->base_block->base_block->ProcessBlockData();
//            }
//        }
//    }
    switch(this->block_type){
    case(BlockType::MATH_BLOCK_PRODUCT):{
        this->output->current_value = this->input_1->current_value * this->input_2->current_value;
        break;
    }
    case(BlockType::MATH_BLOCK_DIF):{
        this->output->current_value = this->input_1->current_value - this->input_2->current_value;
        break;
    }
    case(BlockType::MATH_BLOCK_POW):{
        this->output->current_value = std::pow(this->input_1->current_value, this->input_2->current_value);
        break;
    }
    case(BlockType::MATH_BLOCK_DIV):{
        this->output->current_value = this->input_1->current_value / this->input_2->current_value;
        break;
    }
    case(BlockType::MATH_BLOCK_SUM):{
        this->output->current_value = this->input_1->current_value + this->input_2->current_value;
        break;
    }
    }
    this->simpleblock->is_processed = true;
    qDebug() << "MATHBLOCK Signal " << this->block_type << " " << this->output->current_value;
}
