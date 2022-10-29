#ifndef APP2_TIMEBLOCK_H
#define APP2_TIMEBLOCK_H

#include <QtCore/qglobal.h>
#include "app2_customblock.h"
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>
#include <QDoubleValidator>
#include <QTime>

class APP2_timeblock : public APP2_customblock {
private:
    QTime * system_time = nullptr;

public:
    APP2_signalnode * output = nullptr;

    APP2_timeblock(QGraphicsScene * parent);

    void ProcessBlockData(void) override;

public slots:
    void siResetTime(void);
};

#endif // APP2_TIMEBLOCK_H
