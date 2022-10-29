#ifndef APP2_THRESHOLDBLOCK_H
#define APP2_THRESHOLDBLOCK_H

#include <QtCore/qglobal.h>
#include "app2_customblock.h"
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>
#include <QDoubleValidator>

class APP2_thresholdblock : public APP2_customblock {
private:
    QLineEdit * tmp_max_value_line;
    QLineEdit * tmp_min_value_line;

public:
    APP2_signalnode *   output = nullptr;
    APP2_slotnode *     input = nullptr;
    APP2_valuelabel *   min_label = nullptr;
    APP2_valuelabel *   max_label = nullptr;
    float               max = 0;
    float               min = 0;

    APP2_thresholdblock(QGraphicsScene * parent, float max, float min);

    void ProcessBlockData(void) override;

public slots:
    void slSettingsMenuShow(void);

private slots:
    void slSetValues(void){
        QString txt_max = this->tmp_max_value_line->text();
        QString txt_min = this->tmp_min_value_line->text();

        for (auto iter = txt_max.begin(); iter != txt_max.end(); iter++)
            if ((*iter) == ',') (*iter) = '.';
        for (auto iter = txt_min.begin(); iter != txt_min.end(); iter++)
            if ((*iter) == ',') (*iter) = '.';

        float tmp_min = txt_min.toFloat();
        float tmp_max = txt_max.toFloat();
        if (tmp_min > tmp_max) return;

        this->min = tmp_min;
        this->max = tmp_max;


        this->min_label->setValue(this->min);
        this->max_label->setValue(this->max);
    }
};

#endif // APP2_THRESHOLDBLOCK_H
