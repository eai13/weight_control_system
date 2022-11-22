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

private slots:
    void slFullScreen(void) {emit siFullscreen();}
    void slTargetAdd(void);
    void slTargetClear(void){emit siTargetClear();}
    void slTargetRemove(void) {emit siTargetRemove();}
    void slStartTrajectory(void) {emit siStartTrajectory();}
    void slStopTrajectory(void) {emit siStopTrajectory();}
    void slPauseTrajectory(void) {emit siPauseTrajectory();}

    void slSaveTarget(void) {emit siSaveTarget();}
    void slSaveReal(void) {emit siSaveReal();}
    void slUploadTarget(void);
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
};

#endif // PLOT3DCONFIGS_H
