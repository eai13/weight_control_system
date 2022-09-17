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

class Plot2D : public QObject
{
    Q_OBJECT
public:
    explicit Plot2D(QCustomPlot * plot_h, QRadioButton * rb_turn_h, QRadioButton * rb_rads_h, QDial * dial_h, QLineEdit * lineedit_h);

    QList<QColor>   pencolor_buffer;
    QCustomPlot *   plot;

    QRadioButton *  rb_turn;
    QRadioButton *  rb_rads;
    QLineEdit *     lineedit;
    QDial *         dial;

    QMenu           plot_menu;

    void ResetDial(void);
    void ResetPlot(void);

public slots:

    void slSendPosFromDial(void);
    void slProcessDial(int data);
    void slProcessEditLine(void);
    void slSetRads(void);
    void slSetTurns(void);
    void slSetActiveRegister(bool state);
    void slAddData(uint32_t reg, float value);
    void slShowContextMenu(const QPoint & pos);
    void slPlotActive(bool state);

private:

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

    void siSendPos(float pos);
};

#endif // PLOT2D_H
