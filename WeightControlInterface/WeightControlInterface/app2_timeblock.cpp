#include "app2_timeblock.h"

APP2_timeblock::APP2_timeblock(QGraphicsScene * parent){
    this->output = new APP2_signalnode("Time, msec");
    this->signalnodes.push_back(this->output);

    this->mommy_canvas = parent;
    this->mommy_canvas->addItem(output);

    QList<APP2_valuelabel *>vllist;

    this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(this, QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Time"), vllist));
    this->mommy_canvas->addItem(this->resizenode = new APP2_resizenode);

    connect(this->simpleblock, &APP2_simpleblock::siMainBlockDeleted, this->resizenode, &APP2_resizenode::slMainBlockDeleted);
    connect(this->simpleblock, &APP2_simpleblock::siMoved, this->resizenode, &APP2_resizenode::slMoveTo);
    connect(this->resizenode, &APP2_resizenode::siMoved, this->simpleblock, &APP2_simpleblock::slResize);
}

void APP2_timeblock::ProcessBlockData(void){
    this->output->current_value = this->system_time->elapsed();
    qDebug() << "TIMEBLOCK VALUE " << this->output->current_value;
}
