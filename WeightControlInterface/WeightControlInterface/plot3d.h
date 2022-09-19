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

#define POLYGON_Y_SIZE  100
#define POLYGON_X_SIZE  100
#define POLYGON_Z_SIZE  100

#define CM_PER_DOT      2

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

    struct VertexCartesian{
        VertexCartesian(QVector3D Drive1, QVector3D Drive2, QVector3D Drive3, QVector3D Drive4){
            this->Vertex[0] = Drive1; this->Vertex[1] = Drive2; this->Vertex[2] = Drive3; this->Vertex[3] = Drive4;
        }
        QVector3D Vertex[4];
    };

    VertexCartesian * drive_vertex;

    static QVector3D DirectTransform(VertexCartesian Drives, QVector<float> length);
    static QVector<float> InverseTransform(VertexCartesian Drives, QVector3D Object);

private:

    QtDataVisualization::Q3DScatter * plot;

public slots:

private slots:

signals:

};

#endif // PLOT3D_H
