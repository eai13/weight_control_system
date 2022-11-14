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
    QList<APP2_valuelabel *> vllist;
    parent->addItem(this->simpleblock = new APP2_simpleblock(this, QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Custom Block"), vllist));
    parent->addItem(this->resizenode = new APP2_resizenode);
    connect(this->simpleblock, &APP2_simpleblock::siMainBlockDeleted, this->resizenode, &APP2_resizenode::slMainBlockDeleted);
    connect(this->simpleblock, &APP2_simpleblock::siMoved, this->resizenode, &APP2_resizenode::slMoveTo);
    connect(this->resizenode, &APP2_resizenode::siMoved, this->simpleblock, &APP2_simpleblock::slResize);
}

void APP2_simpleblock::slDeleteThis(void){
    emit this->base_block->siBlockRemoved(this->base_block);
    emit this->siMainBlockDeleted();
    this->deleteLater();
    this->~APP2_simpleblock();
}

void APP2_connectline::ClearFromSignal(void){
    this->slot->connect_line = nullptr;
    this->sig = nullptr;
    this->slot = nullptr;
    this->scene()->removeItem(this);
    this->~APP2_connectline();
}

void APP2_connectline::ClearFromSlot(void){
    for (auto iter = this->sig->signal_lines.begin(); iter != this->sig->signal_lines.end(); iter++){
        if ((*iter) == this){
            this->sig->signal_lines.removeOne((*iter));
            this->sig = nullptr;
            this->slot = nullptr;
            this->scene()->removeItem(this);
            this->~APP2_connectline();
            break;
        }
    }
}

void APP2_connectline::slDeleteThis(void){
    for (auto iter = this->sig->signal_lines.begin(); iter != this->sig->signal_lines.end(); iter++){
        if ((*iter) == this){
            this->sig->signal_lines.removeOne((*iter));
            this->sig = nullptr;
            break;
        }
    }
    this->slot->connect_line = nullptr;
    this->slot = nullptr;
    this->scene()->removeItem(this);
    this->~APP2_connectline();
}

void APP2_slotnode::GetSignal(float val){
    Q_UNUSED(val);
//    this->previous_value = this->current_value;
//    this->current_value = val;
//    this->is_processed = true;
//    this->base_block->base_block->ProcessBlockData();
}
