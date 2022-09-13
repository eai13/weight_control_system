#ifndef PLOT3D_H
#define PLOT3D_H

#include <QtCore/qglobal.h>
#include <QtDataVisualization/q3dscatter.h>
#include <QGroupBox>
#include <QWidget>
#include <QLayout>
#include <QFont>
#include <QVector3D>
#include <QObject>

class Plot3D : public QObject
{
    Q_OBJECT
public:
    Plot3D(QGroupBox * parent);

    void AddRealPoint(double x, double y, double z);
    void AddRealPoint(QVector3D pt);
    void AddTargetPoint(double x, double y, double z);
    void AddTargetPoint(QVector3D pt);
    void ClearReal(void);
    void ClearTarget(void);

    void BuildTargetTrajectory(QVector3D start, QVector3D end);
private:
    QtDataVisualization::Q3DScatter * plot;

private slots:

    void slClicked(void);
};

#endif // PLOT3D_H
