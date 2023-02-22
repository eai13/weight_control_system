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
    float GetAngleFromLength(float length){
        double a = this->trA_calib; double b = this->trB_calib; double c = this->trC_calib - length;
        double D = std::pow(b, 2) - 4 * a * c;
        double angle = ((-1) * b + std::sqrt(D)) / 2 / a;
        return angle;
    }
    float GetLengthFromAngle(float angle){
        float length = this->trA_calib * std::pow(angle, 2) + this->trB_calib * angle + this->trC_calib;
        return length;
    }

    explicit Plot2D(QString title, QCustomPlot * plot_h,
                    QRadioButton * rb_turn_h, QRadioButton * rb_rads_h, QRadioButton * rb_length_h,
                    QDial * dial_h,
                    QLineEdit * lineedit_h,
                    QPushButton * zerocalib, QPushButton * stop, QPushButton * setpos,
                    QPushButton * less_small, QPushButton * less_much, QPushButton * more_small, QPushButton * more_much,
                    double A_cal, double B_cal, double C_cal, double MIN_cal, double MAX_cal);

    QList<QColor>   pencolor_buffer;
    QWidget *       plot_parent;
    QCustomPlot *   plot;

    QRadioButton *  rb_turn;
    QRadioButton *  rb_rads;
    QRadioButton *  rb_length;
    QLineEdit *     lineedit;
    bool            lineedit_pressed = false;
    QDial *         dial;
    bool            dial_pressed = false;
    QPushButton *   pb_calibzero;
    QPushButton *   pb_stop;
    QPushButton *   pb_setpos;
    QPushButton *   pb_jogless_s;
    QPushButton *   pb_jogless_b;
    QPushButton *   pb_jogmore_s;
    QPushButton *   pb_jogmore_b;

    QLineEdit *     lineedit_setpos;

    QMenu           plot_menu;

    float           actual_position_rads;
    float           setpoint_position_rads;

    double trA_calib;
    double trB_calib;
    double trC_calib;
    double MIN_calib;
    double MAX_calib;

    void ResetDial(void);
    void ResetPlot(void);

public slots:

    void slSendSetPos(void);
    void slSetPos(void);
    void slSaveImage(void);
    void slSaveData(void);
    void slRescaleToDefault(void);
    void slSendPosFromDial(void);
    void slProcessDial(int data);
    void slProcessEditLine(void);
    void slSetActiveRegister(bool state);
    void slAddData(uint32_t reg, float value);
    void slShowContextMenu(const QPoint & pos);
    void slPlotActive(bool state);

    void slFullscreen(bool state);
    void slFullscreenQuit(void);

    void slBlockModule(void);
    void slEnableModule(void);

    void slReceiveActualPosition(float fb_rads, float sp_rads);

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
    void siSendSetPos(float pos);
    void siStopDrive(void);

private slots:
    void slCalibrateZero(void);
    void slStopDrive(void);
    void slJogLessSmall(void);
    void slJogLessMuch(void);
    void slJogMoreSmall(void);
    void slJogMoreMuch(void);
};

#endif // PLOT2D_H
