#ifndef SYSTEMCONTROL_H
#define SYSTEMCONTROL_H

#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QWidget>
#endif

namespace Ui {
class SystemControl;
}

class SystemControl : public QWidget
{
    Q_OBJECT

public:
    explicit SystemControl(QWidget *parent = nullptr);
    ~SystemControl();

private:
    Ui::SystemControl *ui;
};

#endif // SYSTEMCONTROL_H
