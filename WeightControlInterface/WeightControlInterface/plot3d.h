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

class Plot3D : public QObject
{
    Q_OBJECT
public:
    explicit Plot3D(QGroupBox *parent = nullptr);

    void AddRealPoint(double x, double y, double z);
    void AddRealPoint(QVector3D pt);
    void AddTargetPoint(double x, double y, double z);
    void AddTargetPoint(QVector3D pt);
    void ClearReal(void);
    void ClearTarget(void);

    void BuildTargetTrajectory(QVector3D start, QVector3D end);

private:

    QtDataVisualization::Q3DScatter * plot;

public slots:

private slots:

signals:

};

#endif // PLOT3D_H
