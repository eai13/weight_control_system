#ifndef APP2_DEADZONEBLOCK_H
#define APP2_DEADZONEBLOCK_H

#include <QtCore/qglobal.h>
#include "app2_customblock.h"
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>
#include <QDoubleValidator>

class APP2_deadzoneblock : public APP2_customblock {
private:
    QLineEdit * tmp_upper_line = nullptr;
    QLineEdit * tmp_lower_line = nullptr;

public:
    APP2_signalnode *   output = nullptr;
    APP2_slotnode *     input = nullptr;
    APP2_valuelabel *   upper_line_label = nullptr;
    APP2_valuelabel *   lower_line_label = nullptr;
    float               upper_line = 0;
    float               lower_line = 0;

    APP2_deadzoneblock(QGraphicsScene * parent, float upper, float lower);

    void ProcessBlockData(void) override;

public slots:
    void slSettingsMenuShow(void);

private slots:
    void slSetValues(void){
        QString txt_upper_line = this->tmp_upper_line->text();
        QString txt_lower_line = this->tmp_lower_line->text();

        for (auto iter = txt_upper_line.begin(); iter != txt_upper_line.end(); iter++)
            if ((*iter) == ',') (*iter) = '.';
        for (auto iter = txt_lower_line.begin(); iter != txt_lower_line.end(); iter++)
            if ((*iter) == ',') (*iter) = '.';

        float tmp_low = txt_lower_line.toFloat();
        float tmp_up = txt_upper_line.toFloat();
        if (tmp_low > tmp_up) return;

        this->lower_line = tmp_low;
        this->upper_line = tmp_up;
        this->lower_line_label->setValue(this->lower_line);
        this->upper_line_label->setValue(this->upper_line);
    }
};

#endif // APP2_DEADZONEBLOCK_H
