#ifndef APP2_PLOTBLOCK_H
#define APP2_PLOTBLOCK_H

#include <QtCore/qglobal.h>
#include "app2_customblock.h"
#include "qcustomplot.h"
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>
#include <QComboBox>
#include <QList>

class APP2_plotblock : public APP2_customblock {
private:
    QCustomPlot *   plot;
    QLineEdit * tmp_data_amount = nullptr;
    QList<QColor> pencolor_buffer;

public:
    APP2_plotblock(QGraphicsScene * parent);
    void ProcessBlockData(void) override;

public slots:
    void slSettingsMenuShow(void);

private slots:
    void slSetValues(void){

    }
    void slSetSlotsAmount(QString amount_str){
        uint8_t amount = amount_str.toUInt();
        while(amount < this->plot->graphCount()){
            this->simpleblock->RemoveSlot();
            this->slotnodes.pop_back();
            this->pencolor_buffer.push_back(this->plot->graph(this->plot->graphCount() - 1)->pen().color());
            this->plot->removeGraph(this->plot->graphCount() - 1);
        }
        while(amount > this->plot->graphCount()){
            this->plot->addGraph();
            this->plot->graph(this->plot->graphCount() - 1)->setPen(this->pencolor_buffer.takeLast());
            this->slotnodes.push_back(new APP2_slotnode("Data"));
            this->mommy_canvas->addItem(this->slotnodes.back());
            this->simpleblock->AddSlot(this->slotnodes.back());
        }
        this->plot->rescaleAxes();
        this->plot->replot();
    }
    void slSavePlotImage(void){
        QString filter;
        QString fname = QFileDialog::getSaveFileName(
                    nullptr, "Save file", QDir::currentPath(), "PNG (*.png);;BMP (*.bmp);;JPG (*.jpg)", &filter);
        if (filter == "PNG (*.png)")
            this->plot->savePng(fname, 1920, 1080);
        else if (filter == "BMP (*.bmp)")
            this->plot->saveBmp(fname, 1920, 1080);
        else if (filter == "JPG (*.jpg)")
            this->plot->saveJpg(fname, 1920, 1080);
    }
};

#endif // APP2_PLOTBLOCK_H
