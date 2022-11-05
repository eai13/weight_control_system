#ifndef PLOT2D_H
#define PLOT2D_H

#include <QtCore/QObject>
#include <QtCore/qglobal.h>

#include "qcustomplot.h"
#include <QList>
#include <QColor>
#include <QDial>
#include <QRadioButton>
#include <QLineEdit>
#include <QMenu>
#include <QTime>
#include <QDoubleValidator>
#include <QPoint>
#include <QMap>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include "global_config.h"

class Plot2D : public QObject
{
    Q_OBJECT
public:
    explicit Plot2D(QString title, QCustomPlot * plot_h,
                    QRadioButton * rb_turn_h, QRadioButton * rb_rads_h, QRadioButton * rb_length_h,
                    QDial * dial_h,
                    QLineEdit * lineedit_h,
                    QPushButton * zerocalib, QPushButton * movezero,
                    double R0, double K, double MIN, double MAX);

    QList<QColor>   pencolor_buffer;
    QWidget *       plot_parent;
    QCustomPlot *   plot;

    QRadioButton *  rb_turn;
    QRadioButton *  rb_rads;
    QRadioButton *  rb_length;
    QLineEdit *     lineedit;
    QDial *         dial;
    QPushButton *   pb_calibzero;
    QPushButton *   pb_movezero;

    QMenu           plot_menu;

    double R0_calib;
    double K_calib;
    double MAX_calib;
    double MIN_calib;

    void ResetDial(void);
    void ResetPlot(void);

public slots:

    void slSaveImage(void);
    void slSaveData(void);
    void slSendPosFromDial(void);
    void slProcessDial(int data);
    void slProcessEditLine(void);
    void slSetRads(void);
    void slSetTurns(void);
    void slSetLength(void);
    void slSetActiveRegister(bool state);
    void slAddData(uint32_t reg, float value);
    void slShowContextMenu(const QPoint & pos);
    void slPlotActive(bool state);

    void slFullscreen(bool state);
    void slFullscreenQuit(void);

private:

    QMainWindow * fullscreen;

    QMap<uint16_t, QString> register_names = {
        { 1,    "Position Setpoint, rad" },
        { 2,    "Position, rad" },
        { 10,   "Velocity Setpoint, rad/s" },
        { 11,   "Velocity, rad/s" },
        { 19,   "Current Setpoint, A" },
        { 20,   "Current, A" },
        { 28,   "Output, V" }
    };

    QColor GrabColor(void){
        return this->pencolor_buffer.takeFirst();
    }

    void ReturnColor(QColor c){
        this->pencolor_buffer.push_back(c);
    }

    struct Register{
        Register(uint32_t r, QCPGraph * id) { this->reg = r; this->plot_id = id; }
        uint32_t    reg;
        QCPGraph *  plot_id;
    };

    QList<Register> active_registers;
    int32_t dial_old_value   = 0;
    int16_t dial_counter     = 0;

    QTime * system_time = nullptr;

signals:
    void siCalibrateZero(void);
    void siSendPos(float pos);

private slots:
    void slCalibrateZero(void){
        emit this->siCalibrateZero();
    }
    void slSetZero(void){
        this->lineedit->setText("0.00");
        this->slProcessEditLine();
    }

};

#endif // PLOT2D_H
