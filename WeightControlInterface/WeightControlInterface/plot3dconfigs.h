#ifndef PLOT3DCONFIGS_H
#define PLOT3DCONFIGS_H

#include <QVector3D>
#include <QtCore/qglobal.h>
#include <QtCore/QObject>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>

//#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
//#else
//#include <QtGui/QWidget>
//#endif

namespace Ui {
class Plot3DConfigs;
}

class Plot3DConfigs : public QWidget
{
    Q_OBJECT

public:
    explicit Plot3DConfigs(QWidget *parent = nullptr);
    ~Plot3DConfigs();

    Ui::Plot3DConfigs *ui;

    void Reset(void);

private:

public slots:
    void slUpdateBar(int perc);
    void slEnableStart(uint8_t state);
    void slEnableStop(uint8_t state);
    void slEnableAbort(uint8_t state);
    void slEnableFullscreen(void);

private slots:
    void slFullScreen(void);
    void slTargetAdd(void);
    void slTargetClear(void){emit siTargetClear();}
    void slTargetRemove(void) {emit siTargetRemove();}
    void slStartTrajectory(void) {emit siStartTrajectory();}
    void slStopTrajectory(void) {emit siStopTrajectory();}
    void slPauseTrajectory(void) {emit siPauseTrajectory();}

    void slSaveTarget(void) {emit siSaveTarget();}
    void slSaveReal(void) {emit siSaveReal();}
    void slUploadTarget(void);

    void slJogXPlusSMALL(void)  { emit siSendObjectStep(QVector3D(0.02, 0, 0)); }
    void slJogXPlusBIG(void)    { emit siSendObjectStep(QVector3D(0.10, 0, 0)); }
    void slJogXMinusSMALL(void) { emit siSendObjectStep(QVector3D(-0.02, 0, 0)); }
    void slJogXMinusBIG(void)   { emit siSendObjectStep(QVector3D(-0.10, 0, 0)); }

    void slJogYPlusSMALL(void)  { emit siSendObjectStep(QVector3D(0, 0.02, 0)); }
    void slJogYPlusBIG(void)    { emit siSendObjectStep(QVector3D(0, 0.10, 0)); }
    void slJogYMinusSMALL(void) { emit siSendObjectStep(QVector3D(0, -0.02, 0)); }
    void slJogYMinusBIG(void)   { emit siSendObjectStep(QVector3D(0, -0.10, 0)); }

    void slJogZPlusSMALL(void)  { emit siSendObjectStep(QVector3D(0, 0, 0.02)); }
    void slJogZPlusBIG(void)    { emit siSendObjectStep(QVector3D(0, 0, 0.10)); }
    void slJogZMinusSMALL(void) { emit siSendObjectStep(QVector3D(0, 0, -0.02)); }
    void slJogZMinusBIG(void)   { emit siSendObjectStep(QVector3D(0, 0, -0.10)); }
signals:
    void siTargetAdd(QVector3D data);
    void siTargetClear(void);
    void siTargetRemove(void);
    void siFullscreen(void);

    void siStartTrajectory(void);
    void siStopTrajectory(void);
    void siPauseTrajectory(void);

    void siSaveTarget(void);
    void siSaveReal(void);
    void siUploadTarget(uint8_t format);

    void siSendObjectStep(QVector3D step);
};

#endif // PLOT3DCONFIGS_H
