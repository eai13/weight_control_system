#include "app2_customblock.h"

APP2_customblock::APP2_customblock(QGraphicsScene * parent, QList<APP2_signalnode *> signal, QList<APP2_slotnode *> slot){
    this->mommy_canvas = parent;
    this->signalnodes = signal;
    this->slotnodes = slot;
    for (auto iter = this->slotnodes.begin(); iter != this->slotnodes.end(); iter++){
        this->mommy_canvas->addItem(*iter);
    }
    for (auto iter = this->signalnodes.begin(); iter != this->signalnodes.end(); iter++){
        this->mommy_canvas->addItem(*iter);
    }
    parent->addItem(this->simpleblock = new APP2_simpleblock(QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes));
    parent->addItem(this->resizenode = new APP2_resizenode);
    connect(this->simpleblock, &APP2_simpleblock::siMoved, this->resizenode, &APP2_resizenode::slMoveTo);
    connect(this->resizenode, &APP2_resizenode::siMoved, this->simpleblock, &APP2_simpleblock::slResize);
}

APP2_constantblock::APP2_constantblock(QGraphicsScene * parent, float value){
    this->output = new APP2_signalnode();
    this->signalnodes.push_back(this->output);
    this->signalnodes.push_back(new APP2_signalnode);

    this->slotnodes.push_back(new APP2_slotnode);
    this->slotnodes.push_back(new APP2_slotnode);

    this->mommy_canvas = parent;
    for (auto iter = this->signalnodes.begin(); iter != this->signalnodes.end(); iter++){
        this->mommy_canvas->addItem(*iter);
    }
    for (auto iter = this->slotnodes.begin(); iter != this->slotnodes.end(); iter++){
        this->mommy_canvas->addItem(*iter);
    }

    this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes));
    this->mommy_canvas->addItem(this->resizenode = new APP2_resizenode);

    connect(this->simpleblock, &APP2_simpleblock::siMoved, this->resizenode, &APP2_resizenode::slMoveTo);
    connect(this->resizenode, &APP2_resizenode::siMoved, this->simpleblock, &APP2_simpleblock::slResize);
    this->value = value;
}
