#include "app2_customblock.h"

APP2_customblock::APP2_customblock(QGraphicsScene * parent){
    parent->addItem(this->simpleblock = new APP2_simpleblock(QRectF(0, 0, 100, 100)));
    parent->addItem(this->resizenode = new APP2_resizenode);
    connect(this->simpleblock, &APP2_simpleblock::siMoved, this->resizenode, &APP2_resizenode::slMoveTo);
    connect(this->resizenode, &APP2_resizenode::siMoved, this->simpleblock, &APP2_simpleblock::slResize);
}

//void APP2_customblock::mousePressEvent(QGraphicsSceneMouseEvent *event){
//    QGraphicsItem::mousePressEvent(event);
//}

//void APP2_customblock::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
//    QGraphicsItem::mouseReleaseEvent(event);
//}
