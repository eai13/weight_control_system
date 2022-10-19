#ifndef APP2_CONSTBLOCK_H
#define APP2_CONSTBLOCK_H

#include <QtCore/qglobal.h>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>
#include <QCursor>
#include <QMenu>
#include <QGraphicsLineItem>
#include <QGraphicsSceneEvent>
#include "app2_resizenode.h"

#include <iostream>

class APP2_simpleblock;
class APP2_constantblock;
class APP2_resizenode;
class APP2_signalnode;
class APP2_slotnode;
class APP2_connectline;

static APP2_connectline * connect_line_buf = nullptr;

/* @brief
 * Connection Line object
 */

class APP2_connectline : public QObject, public QGraphicsItem {
    Q_OBJECT

public:
    QPen pen;
    QRectF size;
    QLineF line;
    void SetPen(QPen new_pen){
        this->pen = new_pen;
    }

    void SetStart(QPointF new_dot){
        this->prepareGeometryChange();
        this->setPos(new_dot + QPointF(10, 5));
        this->line.setP1(new_dot);
    }
    void SetEnd(QPointF new_dot){
        this->prepareGeometryChange();
        this->line.setP2(new_dot);
    }

    APP2_connectline(QPointF start, QPointF end){
        qDebug() << this->acceptedMouseButtons();
        this->setCursor(QCursor(Qt::CrossCursor));
        this->setZValue(-5);
        this->pen.setWidth(2);
        this->pen.setColor(Qt::black);
        this->setPos(start.x() + 10, start.y() + 5);
        this->line = QLineF(start, end);
    }

    QRectF boundingRect() const override {
        return QRectF(0, 0, this->line.dx(), this->line.dy());
    }
    QPainterPath shape() const override{
        QPainterPath path;
        QVector<QPointF> tmp;
        if (this->line.dy() > 0){
            path.addRect(QRectF(QPointF(-3, -3), QPointF(this->line.dx() / 2 + 3, 6)));
            path.addRect(QRectF(QPointF(this->line.dx() / 2 - 3, -3), QPointF(this->line.dx() / 2 + 3, this->line.dy() + 3)));
            path.addRect(QRectF(QPointF(this->line.dx() / 2 - 3, this->line.dy() - 3), QPointF(this->line.dx() + 3, this->line.dy() + 3)));
        }
        else{
            tmp.push_back(QPointF(0, 0)); tmp.push_back(QPointF(0, -6)); tmp.push_back(QPointF(this->line.dx() / 2 - 6, -6));
            tmp.push_back(QPointF(this->line.dx() / 2 - 6, this->line.dy())); tmp.push_back(QPointF(this->line.dx(), this->line.dy()));
            tmp.push_back(QPointF(this->line.dx(), this->line.dy() + 6)); tmp.push_back(QPointF(this->line.dx() / 2 + 6, this->line.dy() + 6));
            tmp.push_back(QPointF(this->line.dx() / 2 + 6, 0)); tmp.push_back(QPointF(0, 0));
            path.addPolygon(QPolygonF(tmp));
        }
        return path;
    }
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) override{
        painter->setPen(this->pen);
        painter->drawLine(0, 0, this->line.dx() / 2, 0);
        painter->drawLine(this->line.dx() / 2, 0, this->line.dx() / 2, this->line.dy());
        painter->drawLine(this->line.dx() / 2, this->line.dy(), this->line.dx(), this->line.dy());
        this->scene()->update();
    }

    APP2_signalnode *   sig = nullptr;
    APP2_slotnode *     slot = nullptr;

    void ClearFromSignal(void);
    void ClearFromSlot(void);

    ~APP2_connectline(void){}

public slots:
    void slDeleteThis(void);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event){
        if (event->button() == Qt::RightButton){
            QMenu menu;
            menu.addAction("Delete", this, &APP2_connectline::slDeleteThis);
            menu.exec(QCursor::pos());
            QGraphicsItem::mousePressEvent(event);
        }
    }

};

/* @brief
 * Slot node for the object
 */

class APP2_slotnode : public QObject, public QGraphicsItem {
    Q_OBJECT

private:
    QRectF size;

public:

    APP2_connectline * connect_line = nullptr;

    void SetTip(QString tip){
        this->setToolTip("Slot::" + tip);
    }

    APP2_slotnode(QString tip){
        this->setToolTip("Slot::" + tip);
        this->setCursor(QCursor(Qt::CrossCursor));
        this->size.setX(0);
        this->size.setY(0);
        this->size.setHeight(30);
        this->size.setWidth(30);
    }
    APP2_slotnode(void){
        this->setToolTip("Slot::");
        this->setCursor(QCursor(Qt::CrossCursor));
        this->size.setX(0);
        this->size.setY(0);
        this->size.setHeight(30);
        this->size.setWidth(30);
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
        QPointF pts[5] = { QPointF(10, 10), QPointF(20, 10), QPointF(20, 20), QPointF(10, 20), QPointF(15, 15) };
        painter->drawPolygon(pts, 5);
    }

    bool flClicked = false;

    ~APP2_slotnode(void){}

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event){
        if (event->button() == Qt::LeftButton){

        }
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event){
        if (event->button() == Qt::LeftButton){

        }
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event){
        if (this->flClicked){

        }
        else{

        }
    }

public slots:
    void slMoveTo(QPointF new_pos){
        this->setPos(new_pos);
        if (this->connect_line != nullptr){
            this->connect_line->SetEnd(new_pos + QPointF(5, 10));
        }
    }
    void slMainBlockDeleted(void){
        if (this->connect_line != nullptr)
            this->connect_line->ClearFromSlot();
        this->scene()->removeItem(this);
        this->~APP2_slotnode();
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
    QList<APP2_connectline *> signal_lines;

    void SetTip(QString tip){
        this->setToolTip("Signal::" + tip);
    }

    APP2_signalnode(QString tip){
        this->setToolTip("Signal::" + tip);
        this->setCursor(QCursor(Qt::CrossCursor));
        this->size.setX(0);
        this->size.setY(0);
        this->size.setHeight(30);
        this->size.setWidth(30);
    }
    APP2_signalnode(void){
        this->setToolTip("Signal::");
        this->setCursor(QCursor(Qt::CrossCursor));
        this->size.setX(0);
        this->size.setY(0);
        this->size.setHeight(30);
        this->size.setWidth(30);
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
        QPointF pts[5] = { QPointF(10, 10), QPointF(15, 10), QPointF(20, 15), QPointF(15, 20), QPointF(10, 20) };
        painter->drawPolygon(pts, 5);
    }

    bool flClicked = false;

    ~APP2_signalnode(void){}

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event){
        if (event->button() == Qt::LeftButton){
            this->flClicked = true;
            APP2_connectline * tmp = new APP2_connectline(this->pos() + QPointF(10, 10), this->pos());
            tmp->SetPen(QPen(Qt::green, 2));
            this->signal_lines.push_back(tmp);
            this->signal_lines.back()->sig = this;
            this->scene()->addItem(this->signal_lines.back());
        }
    }
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event){
        if (event->button() == Qt::LeftButton){
            this->flClicked = false;
            if (this->signal_lines.back()->slot == nullptr){
                this->scene()->removeItem(this->signal_lines.back());
                this->signal_lines.pop_back();
            }
            else{
                this->signal_lines.back()->slot->connect_line = this->signal_lines.back();
                this->signal_lines.back()->SetPen(QPen(Qt::black, 2));
            }
        }
    }
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event){
        if (this->flClicked){
            QGraphicsItem * item;
            if ((item = this->scene()->itemAt(event->scenePos(), QTransform())) != nullptr){
                if (item->toolTip().startsWith("Slot::")){
                    // Some shitty stuff, nevermind
                    int * tmp = reinterpret_cast<int *>(item);
                    tmp -= 4;
                    // Some shitty stuff, nevermind
                    for (auto iter = this->signal_lines.begin(); iter != this->signal_lines.end(); iter++){
                        if ((reinterpret_cast<APP2_slotnode *>(tmp)->connect_line != nullptr) || (*iter)->slot == reinterpret_cast<APP2_slotnode *>(tmp)){
                            return;
                        }
                    }
                    this->signal_lines.back()->SetEnd(item->pos() + QPointF(5, 10));
                    this->signal_lines.back()->slot = reinterpret_cast<APP2_slotnode *>(tmp);
                }
                else{
                    this->signal_lines.back()->SetEnd(event->scenePos());
                    this->signal_lines.back()->slot = nullptr;
                }
            }
            else{
                this->signal_lines.back()->SetEnd(event->scenePos());
                this->signal_lines.back()->slot = nullptr;
            }
        }
    }

public slots:
    void slMoveTo(QPointF new_pos){
        this->setPos(new_pos);
        for (auto iter = this->signal_lines.begin(); iter != this->signal_lines.end(); iter++){
            (*iter)->SetStart(new_pos + QPointF(10, 10));
        }
    }
    void slMainBlockDeleted(void){
        for (auto iter = this->signal_lines.begin(); iter != this->signal_lines.end(); iter++){
            (*iter)->ClearFromSignal();
        }
        this->scene()->removeItem(this);
        this->~APP2_signalnode();
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
    QGraphicsTextItem *         type = nullptr;
    QGraphicsTextItem *         name = nullptr;

    void SigSlotRefresh(void){
        this->type->setPos(this->pos() + QPointF(0, -20));
        uint16_t it = 0;
        for (auto iter = this->signalnodes.begin(); iter != this->signalnodes.end(); iter++){
            (*iter)->slMoveTo(this->pos() + QPointF(this->size.width() - 13, this->size.height() / (this->signalnodes.count() + 1) * (it++ + 1) - 10));
        }
        it = 0;
        for (auto iter = this->slotnodes.begin(); iter != this->slotnodes.end(); iter++){
            (*iter)->slMoveTo(this->pos() + QPointF(-17, this->size.height() / (this->slotnodes.count() + 1) * (it++ + 1) - 10));
        }
    }

public:
    void SetTip(QString tip){
        this->setToolTip("Block::" + tip);
    }

    APP2_simpleblock(QRectF size, QList<APP2_signalnode *> sigs, QList<APP2_slotnode *> slts, QGraphicsTextItem * type){
        this->setToolTip("Block::");
        this->setZValue(-1);

        this->type = type;

        this->size = size;
        this->signalnodes = sigs;
        for (auto iter = this->signalnodes.begin(); iter != this->signalnodes.end(); iter++){
            connect(this, &APP2_simpleblock::siMainBlockDeleted, (*iter), &APP2_signalnode::slMainBlockDeleted);
        }
        this->slotnodes = slts;
        for (auto iter = this->slotnodes.begin(); iter != this->slotnodes.end(); iter++){
            connect(this, &APP2_simpleblock::siMainBlockDeleted, (*iter), &APP2_slotnode::slMainBlockDeleted);
        }

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

    ~APP2_simpleblock(void){
        if (this->type != nullptr) this->scene()->removeItem(this->type);
        if (this->name != nullptr) this->scene()->removeItem(this->name);
        this->scene()->removeItem(this);
    }

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
            menu.addAction("Delete", this, &APP2_simpleblock::slDeleteThis);
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
    void slDeleteThis(void){
        emit this->siMainBlockDeleted();
        this->deleteLater();
        this->~APP2_simpleblock();
    }

signals:
    void siMainBlockDeleted(void);
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

    ~APP2_resizenode(void){
        qDebug() << "Resize Node Removed";
        this->scene()->removeItem(this);
    }

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
    void slMainBlockDeleted(void){
        qDebug() << "signal received by sig node";
        this->~APP2_resizenode();
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
    ~APP2_customblock(void){
        delete this->simpleblock;
        delete this->resizenode;
    }
protected:

signals:

};

#endif // APP2_CONSTBLOCK_H





















