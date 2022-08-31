#ifndef SYSTEMCONTROL_H
#define SYSTEMCONTROL_H

#include "global_config.h"

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

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
    Ui::SystemControl * ui;
    QSerialPort *       serial;

private slots:


public slots:
    void slReceiveSerial(QSerialPort * p_serial){
        this->serial = p_serial;
    }

signals:
    void siSendSerial(QSerialPort * p_serial);

};

#endif // SYSTEMCONTROL_H
