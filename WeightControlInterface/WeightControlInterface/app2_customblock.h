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

/* @brief
 * basic block class
 */

class APP2_simpleblock : public QObject, public QGraphicsItem {
    Q_OBJECT

private:
    QRectF size;

public:
    APP2_simpleblock(QRectF size){
        this->setCursor(QCursor(Qt::PointingHandCursor));
        this->setFlag(QGraphicsItem::ItemIsMovable);
        this->size = size;
    }
    QRectF boundingRect() const override{
        return QRectF(this->size);
    }
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) override{
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
        }
        else{
            event->ignore();
        }
    }

public slots:

    void slResize(QPointF new_size){
        QPointF deviation = this->pos() - new_size;
        this->setPos(new_size);
        this->size.setWidth(this->size.width() + deviation.x());
        this->size.setHeight(this->size.height() + deviation.y());
    }

signals:

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
 * Signal node for the object
 */

class APP2_signalnode : public QObject, public QGraphicsItem {
    Q_OBJECT

};

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
        painter->setBrush(Qt::SolidPattern);
        QPointF pts[3] = { QPointF(0, 0), QPointF(10, 5), QPointF(0, 10) };
        painter->drawPolygon(pts, 3);
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
    void slMoveTo(QPointF deviation){
        this->setPos(this->pos() + deviation);
    }

signals:

};

class APP2_customblock : public QObject {
    Q_OBJECT

public:
    APP2_simpleblock *  simpleblock;
    APP2_resizenode *   resizenode;
    APP2_slotnode *     slotnode;

public:
    APP2_customblock(QGraphicsScene * parent);

protected:

signals:

};

#endif // APP2_CONSTBLOCK_H
