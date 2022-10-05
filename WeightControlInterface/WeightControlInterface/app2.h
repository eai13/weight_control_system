#ifndef APP2_H
#define APP2_H

#include <QPoint>
#include <QDebug>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include "app2_customblock.h"
#include "app2_constantblock.h"
#include "app2_mathoperationblock.h"
#include <QList>
#include <QMenu>

//#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
//#else
//#include <QtGui/QWidget>
//#endif

namespace Ui {
class APP2;
}

class APP2 : public QWidget
{
    Q_OBJECT

public:
    explicit APP2(QWidget *parent = nullptr);
    ~APP2();

private:
    Ui::APP2 *ui;

    QMenu canvas_menu;

    QList<APP2_customblock *> block_list;

public slots:

    void slAddItem(const QModelIndex & index);
};

#endif // APP2_H
