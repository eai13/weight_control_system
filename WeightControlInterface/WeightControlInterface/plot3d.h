#ifndef PLOT3D_H
#define PLOT3D_H

#include <QtCore/QObject>
#include <QtCore/qglobal.h>
#include <QtDataVisualization/q3dscatter.h>
#include <QGroupBox>
#include <QtWidgets/QWidget>
#include <QLayout>
#include <QFont>
#include <QVector3D>
#include <math.h>
#include <QMainWindow>
#include <QPushButton>
#include "global_config.h"

#define DEFAULT_MAP_HEIGHT  ((float)(2.00))
#define DEFAULT_MAP_WIDTH   ((float)(2.70))
#define DEFAULT_MAP_LENGTH  ((float)(3.30))

class Plot3D : public QObject
{
    Q_OBJECT
public:

    explicit Plot3D(QGroupBox *parent = nullptr);

    void AddRealPoint(double x, double y, double z);
    void AddRealPoint(QVector3D pt);
    void AddTargetPoint(double x, double y, double z);
    void AddTargetPoint(QVector3D pt);
    void RemoveLastTarget(void);
    void ClearReal(void);
    void ClearTarget(void);

    void BuildTargetTrajectory(QVector3D start, QVector3D end);

    QVector3D DirectTransform(QVector<float> length);
    QVector<float> InverseTransform(QVector3D object);

    float AngleFromLength(float length);
    float LengthFromAngle(float angle);

private:

    float map_width;
    float map_length;
    float map_height;

    QList<QVector3D> trajectory_copy;

    QGridLayout *   fullscr_layout;
    QGroupBox *     group_box_parent;
    QWidget *       plot_widget;
    QMainWindow *   fullscreen;
    QtDataVisualization::Q3DScatter * plot;

public slots:

    void slFullscreen(void);
    void slFullscreenClosed(void);
    void slTargetAdd(QVector3D data);
    void slTargetClear(void);
    void slTargetRemove(void);

    void slStartTrajectory(void);
    void slStopTrajectory(void);
    void slPauseTrajectory(void);

    void slSaveReal(void);
    void slSaveTarget(void);
    void slUploadTarget(uint8_t format);

private slots:

signals:

};

#endif // PLOT3D_H
