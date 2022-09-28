#ifndef APP2_CONSTBLOCK_H
#define APP2_CONSTBLOCK_H

//#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
//#else
//#include <QtGui/QWidget>
//#endif

namespace Ui {
class app2_constblock;
}

class app2_constblock : public QWidget
{
    Q_OBJECT

public:
    explicit app2_constblock(QWidget *parent = nullptr);
    ~app2_constblock();

private:
    Ui::app2_constblock *ui;
};

#endif // APP2_CONSTBLOCK_H
