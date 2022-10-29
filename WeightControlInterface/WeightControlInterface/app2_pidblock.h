#ifndef APP2_PIDBLOCK_H
#define APP2_PIDBLOCK_H

#include <QtCore/qglobal.h>
#include <app2_customblock.h>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QDoubleValidator>

class APP2_PIDblock : public APP2_customblock{

private:
    QLineEdit * tmp_P_line = nullptr;
    QLineEdit * tmp_I_line = nullptr;
    QLineEdit * tmp_D_line = nullptr;

    QCheckBox * tmp_I_antiwindup_check = nullptr;
    QLineEdit * tmp_I_antiwindup_line = nullptr;

public:
    float P = 0, I = 0, D = 0;
    float I_antiwindup;
    float accumulator = 0;
    float prev_error = 0;
    bool I_antiwindup_enabled = false;

    APP2_signalnode * output = nullptr;
    APP2_slotnode * input = nullptr;

    APP2_valuelabel * P_label = nullptr;
    APP2_valuelabel * I_label = nullptr;
    APP2_valuelabel * D_label = nullptr;

    APP2_PIDblock(QGraphicsScene * parent);

    void ProcessBlockData(void) override;

public slots:
    void slSettingsMenuShow(void);

private slots:
    void slSetValue(void){
        QString txt_P = this->tmp_P_line->text();
        QString txt_I = this->tmp_I_line->text();
        QString txt_D = this->tmp_D_line->text();
        QString txt_I_aw = this->tmp_I_antiwindup_line->text();

        if (this->tmp_I_antiwindup_check->isChecked())
            this->I_antiwindup_enabled = true;
        else
            this->I_antiwindup_enabled = false;

        for (auto iter = txt_P.begin(); iter != txt_P.end(); iter++)
            if ((*iter) == ',') (*iter) = '.';
        for (auto iter = txt_I.begin(); iter != txt_I.end(); iter++)
            if ((*iter) == ',') (*iter) = '.';
        for (auto iter = txt_D.begin(); iter != txt_D.end(); iter++)
            if ((*iter) == ',') (*iter) = '.';
        for (auto iter = txt_I_aw.begin(); iter != txt_I_aw.end(); iter++)
            if ((*iter) == ',') (*iter) = '.';

        this->P = txt_P.toDouble();
        this->P_label->setValue(this->P);
        this->I = txt_I.toDouble();
        this->I_label->setValue(this->I);
        this->D = txt_D.toDouble();
        this->D_label->setValue(this->D);
        this->I_antiwindup = txt_I_aw.toDouble();
    }

};

#endif // APP2_PIDBLOCK_H
