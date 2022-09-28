#ifndef APP2_H
#define APP2_H

#include <QPoint>
#include <QDebug>

#include "app2_constblock.h"

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

public slots:
    void slContextMenuRequested(const QPoint & pos);
};

#endif // APP2_H
