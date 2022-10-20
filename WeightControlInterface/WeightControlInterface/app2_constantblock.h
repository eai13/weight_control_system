#ifndef APP2_CONSTANTBLOCK_H
#define APP2_CONSTANTBLOCK_H

#include <QtCore/qglobal.h>
#include "app2_customblock.h"
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>
#include <QDoubleValidator>

class APP2_constantblock : public APP2_customblock{
private:
    QLineEdit * tmp_value_line;

public:
    APP2_signalnode *   output = nullptr;
    APP2_valuelabel *   valuelabel = nullptr;
    float               value = 0;

    APP2_constantblock(QGraphicsScene * parent, float value);

    void ProcessBlockData(void) override;

public slots:
    void slSettingsMenuShow(void);

private slots:
    void slSetValue(void){
        QString txt = this->tmp_value_line->text();
        for (auto iter = txt.begin(); iter != txt.end(); iter++)
            if ((*iter) == ',') (*iter) = '.';
        this->value = txt.toDouble();
        this->valuelabel->setValue(this->value);
    }
protected:

};

#endif // APP2_CONSTANTBLOCK_H
