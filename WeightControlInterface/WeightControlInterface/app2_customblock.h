#ifndef APP2_CONSTBLOCK_H
#define APP2_CONSTBLOCK_H

#include <QtCore/qglobal.h>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>
#include <QCursor>
#include <QMenu>
#include <QGraphicsSceneEvent>
#include "app2_resizenode.h"

class APP2_simpleblock;
class APP2_constantblock;
class APP2_resizenode;
class APP2_signalnode;
class APP2_slotnode;

/* @brief
 * Slot node for the object
 */

class APP2_slotnode : public QObject, public QGraphicsItem {
    Q_OBJECT

private:
    QRectF size;

public:
    APP2_slotnode(void){
        this->setCursor(QCursor(Qt::CrossCursor));
        this->size.setX(0);
        this->size.setY(0);
        this->size.setHeight(10);
        this->size.setWidth(10);
    }
    QRectF boundingRect() const override{
        return QRectF(this->size);
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override{
        painter->setPen(Qt::black);
        QBrush tmp_br = painter->brush();
        tmp_br.setColor(QColor(175, 0, 0));
        tmp_br.setStyle(Qt::SolidPattern);
        painter->setBrush(tmp_br);
        QPointF pts[5] = { QPointF(0, 0), QPointF(10, 0), QPointF(10, 10), QPointF(0, 10), QPointF(5, 5) };
        painter->drawPolygon(pts, 5);
    }

    bool flClicked = false;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event){

    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event){

    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event){

    }

public slots:
    void slMoveTo(QPointF new_pos){
        this->setPos(new_pos);
    }

signals:


};

/* @brief
 * Signal node for the object
 */

class APP2_signalnode : public QObject, public QGraphicsItem {
    Q_OBJECT

private:
    QRectF size;

public:
    APP2_signalnode(void){
        this->setCursor(QCursor(Qt::CrossCursor));
        this->size.setX(0);
        this->size.setY(0);
        this->size.setHeight(10);
        this->size.setWidth(10);
    }
    QRectF boundingRect() const override{
        return QRectF(this->size);
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override{
        painter->setPen(Qt::black);
        QBrush tmp_br = painter->brush();
        tmp_br.setColor(QColor(0, 175, 0));
        tmp_br.setStyle(Qt::SolidPattern);
        painter->setBrush(tmp_br);
        QPointF pts[5] = { QPointF(0, 0), QPointF(5, 0), QPointF(10, 5), QPointF(5, 10), QPointF(0, 10) };
        painter->drawPolygon(pts, 5);
    }

    bool flClicked = false;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event){

    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event){

    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event){

    }

public slots:
    void slMoveTo(QPointF new_pos){
        this->setPos(new_pos);
    }

signals:

};

/* @brief
 * basic block class
 */

class APP2_simpleblock : public QObject, public QGraphicsItem {
    Q_OBJECT

private:
    QRectF size;
    QList<APP2_signalnode *>    signalnodes;
    QList<APP2_slotnode *>      slotnodes;
    QGraphicsTextItem *         type;
    QGraphicsTextItem *         name;

    void SigSlotRefresh(void){
        this->type->setPos(this->pos() + QPointF(0, -20));
        uint16_t it = 0;
        for (auto iter = this->signalnodes.begin(); iter != this->signalnodes.end(); iter++){
            (*iter)->slMoveTo(this->pos() + QPointF(this->size.width() - 3, this->size.height() / (this->signalnodes.count() + 1) * (it++ + 1)));
        }
        it = 0;
        for (auto iter = this->slotnodes.begin(); iter != this->slotnodes.end(); iter++){
            (*iter)->slMoveTo(this->pos() + QPointF(-7, this->size.height() / (this->slotnodes.count() + 1) * (it++ + 1)));
        }
    }

public:
    APP2_simpleblock(QRectF size, QList<APP2_signalnode *> sigs, QList<APP2_slotnode *> slts, QGraphicsTextItem * type){
        this->setZValue(-1);

        this->type = type;

        this->size = size;
        this->signalnodes = sigs;
        this->slotnodes = slts;

        this->SigSlotRefresh();

        this->setCursor(QCursor(Qt::PointingHandCursor));
        this->setFlag(QGraphicsItem::ItemIsMovable);
    }
    QRectF boundingRect() const override{
        return QRectF(this->size);
    }
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) override{
        QBrush br_tmp = painter->brush();
        br_tmp.setColor(Qt::white);
        br_tmp.setStyle(Qt::SolidPattern);
        painter->setBrush(br_tmp);
        painter->drawRect(this->size);
    }

    bool flClicked = false;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event){
        if (event->button() == Qt::LeftButton){
            this->flClicked = true;
            QGraphicsItem::mousePressEvent(event);
        }
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event){
        if (event->button() == Qt::LeftButton){
            this->flClicked = false;
            QGraphicsItem::mouseReleaseEvent(event);
            emit this->siMoved(QRectF(this->pos(), QPointF(this->size.width() + this->pos().x(), this->size.height() + this->pos().y())));
            this->SigSlotRefresh();
        }
        else{
            QMenu menu;
            menu.addAction("Delete");
            menu.exec(QCursor::pos());
        }
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event){
        if (this->flClicked){
            emit this->siMoved(QRectF(this->pos(), QPointF(this->size.width() + this->pos().x(), this->size.height() + this->pos().y())));
            QGraphicsItem::mouseMoveEvent(event);
            this->SigSlotRefresh();
        }
        else{
            event->ignore();
        }
    }
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event){
        emit this->siSettingsMenuShow();
        QGraphicsItem::mouseDoubleClickEvent(event);
    }

public slots:

    void slResize(QPointF new_size){
        QPointF deviation = this->pos() - new_size;
        this->setPos(new_size);
        this->size.setWidth(this->size.width() + deviation.x());
        this->size.setHeight(this->size.height() + deviation.y());
        this->SigSlotRefresh();
    }

signals:
    void siSettingsMenuShow(void);
    void siMoved(QRectF new_pos);
};



/* @brief
 * Resize node class for resize control of the object
 */

class APP2_resizenode : public QObject, public QGraphicsItem {
    Q_OBJECT

private:
    QRectF size;
    QPointF main_block_top_corner;
    QPointF main_block_bottom_corner;
    qreal min_size = 50;
    qreal max_size = 150;

public:
    APP2_resizenode(void){
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
        this->size.setX(0);
        this->size.setY(0);
        this->size.setHeight(10);
        this->size.setWidth(10);
        this->setFlag(QGraphicsItem::ItemIsMovable);
    }
    QRectF boundingRect() const override{
        return QRectF(this->size);
    }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override{
        painter->setBrush(Qt::SolidPattern);
        painter->drawRect(this->size);
    }

    bool flClicked = false;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event){
        this->flClicked = true;
        QGraphicsItem::mousePressEvent(event);
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event){
        this->flClicked = false;
        QGraphicsItem::mouseReleaseEvent(event);
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event){
        if (this->flClicked){
            if (((this->main_block_bottom_corner.x() - (event->pos() + this->pos()).x()) > this->max_size) ||
                ((this->main_block_bottom_corner.y() - (event->pos() + this->pos()).y()) > this->max_size) ||
                ((this->main_block_bottom_corner.x() - (event->pos() + this->pos()).x()) < this->min_size) ||
                ((this->main_block_bottom_corner.y() - (event->pos() + this->pos()).y()) < this->min_size)){
                    event->ignore();
                    return;
            }
            emit this->siMoved(this->pos());
            QGraphicsItem::mouseMoveEvent(event);
        }
        else{
            event->ignore();
        }
    }

public slots:
    void slMoveTo(QRectF new_pos){
        this->setPos(new_pos.topLeft());
        this->main_block_top_corner = new_pos.topLeft();
        this->main_block_bottom_corner = new_pos.bottomRight();
    }

signals:
    void siMoved(QPointF new_position);
};



/* @brief
 * Custom Block for debug
 */

class APP2_customblock : public QObject {
    Q_OBJECT

public:
    QGraphicsScene *            mommy_canvas;
    APP2_simpleblock *          simpleblock;
    APP2_resizenode *           resizenode;
    QList<APP2_signalnode *>    signalnodes;
    QList<APP2_slotnode *>      slotnodes;

public:
    APP2_customblock(void){}
    APP2_customblock(QGraphicsScene * parent, QList<APP2_signalnode *> signal, QList<APP2_slotnode *> slot);

protected:

signals:

};

#endif // APP2_CONSTBLOCK_H





















