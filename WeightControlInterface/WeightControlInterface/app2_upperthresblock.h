#ifndef APP2_UPPERTHRESBLOCK_H
#define APP2_UPPERTHRESBLOCK_H

#include <QtCore/qglobal.h>
#include "app2_customblock.h"
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>
#include <QDoubleValidator>

class APP2_upperthresblock : public APP2_customblock {
private:
    QLineEdit * tmp_thres_line = nullptr;

public:
    APP2_signalnode *   output = nullptr;
    APP2_slotnode *     input = nullptr;
    APP2_valuelabel *   thres_label = nullptr;
    float               thres = 0;

    APP2_upperthresblock(QGraphicsScene * parent, float thres);

    void ProcessBlockData(void) override;

public slots:
    void slSettingsMenuShow(void);

private slots:
    void slSetValues(void){
        QString txt_thres = this->tmp_thres_line->text();

        for (auto iter = txt_thres.begin(); iter != txt_thres.end(); iter++)
            if ((*iter) == ',') (*iter) = '.';

        this->thres = txt_thres.toFloat();
        this->thres_label->setValue(this->thres);
    }
};

#endif // APP2_UPPERTHRESBLOCK_H
