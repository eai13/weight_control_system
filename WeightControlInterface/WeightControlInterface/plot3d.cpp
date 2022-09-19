#include "plot3d.h"
#include <QtCore/qglobal.h>
#include <iostream>

Plot3D::Plot3D(QGroupBox *parent) : QObject(parent){
    this->plot = new QtDataVisualization::Q3DScatter();
    QWidget * plot_widget = QWidget::createWindowContainer(this->plot);
    parent->layout()->addWidget(plot_widget);
    plot_widget->show();

    this->drive_vertex = new VertexCartesian(QVector3D(0, POLYGON_Y_SIZE, POLYGON_Z_SIZE),
                                             QVector3D(POLYGON_X_SIZE, POLYGON_Y_SIZE, POLYGON_Z_SIZE),
                                             QVector3D(0, 0, POLYGON_Z_SIZE),
                                             QVector3D(POLYGON_X_SIZE, 0, POLYGON_Z_SIZE));
    QVector<float> len;
    len.push_back(80); len.push_back(80); len.push_back(80); len.push_back(80);
    qDebug() << this->DirectTransform(*(this->drive_vertex), len);

    this->plot->axisX()->setAutoAdjustRange(false);
    this->plot->axisX()->setRange(0, POLYGON_X_SIZE);
    this->plot->axisY()->setAutoAdjustRange(false);
    this->plot->axisY()->setRange(0, POLYGON_Y_SIZE);
    this->plot->axisZ()->setAutoAdjustRange(false);
    this->plot->axisZ()->setRange(0, POLYGON_Z_SIZE);

    this->plot->activeTheme()->setType(QtDataVisualization::Q3DTheme::ThemePrimaryColors);
    this->plot->activeTheme()->setFont(QFont("Sans Serif", 50, QFont::Weight::Normal));
    this->plot->setShadowQuality(QtDataVisualization::QAbstract3DGraph::ShadowQualityNone);
    this->plot->setTitle("Object Trajectory");

    QtDataVisualization::QScatterDataProxy * target_proxy = new QtDataVisualization::QScatterDataProxy;
    QtDataVisualization::QScatterDataProxy * real_proxy = new QtDataVisualization::QScatterDataProxy;
    QtDataVisualization::QScatter3DSeries * target_series = new QtDataVisualization::QScatter3DSeries(target_proxy);
    QtDataVisualization::QScatter3DSeries * real_series = new QtDataVisualization::QScatter3DSeries(real_proxy);

    target_series->setItemSize(0.1);
    target_series->setItemLabelFormat(QStringLiteral("Target Trajectory: @xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    target_series->setMeshSmooth(true);
    target_series->setBaseColor(QColor(0, 0, 255));
    this->plot->addSeries(target_series);

    real_series->setItemLabelFormat(QStringLiteral("Real Trajectory: @xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    real_series->setMeshSmooth(true);
    real_series->setBaseColor(QColor(255, 0, 0));
    this->plot->addSeries(real_series);

    this->plot->axisX()->setTitle("X");
    this->plot->axisX()->setTitleVisible(true);
    this->plot->axisY()->setTitle("Y");
    this->plot->axisY()->setTitleVisible(true);
    this->plot->axisZ()->setTitle("Z");
    this->plot->axisZ()->setTitleVisible(true);
}

void Plot3D::AddRealPoint(double x, double y, double z){
    this->plot->seriesList().at(1)->dataProxy()->addItem(QtDataVisualization::QScatterDataItem(QVector3D(x, y, z)));
}
void Plot3D::AddRealPoint(QVector3D pt){
    this->plot->seriesList().at(1)->dataProxy()->addItem(QtDataVisualization::QScatterDataItem(pt));
}

void Plot3D::AddTargetPoint(double x, double y, double z){
    this->plot->seriesList().at(0)->dataProxy()->addItem(QtDataVisualization::QScatterDataItem(QVector3D(x, y, z)));
}
void Plot3D::AddTargetPoint(QVector3D pt){
    this->plot->seriesList().at(0)->dataProxy()->addItem(QtDataVisualization::QScatterDataItem(pt));
}

void Plot3D::ClearReal(void){
    this->plot->seriesList().at(1)->dataProxy()->removeItems(0, this->plot->seriesList().at(1)->dataProxy()->itemCount());
}

void Plot3D::ClearTarget(void){
    this->plot->seriesList().at(0)->dataProxy()->removeItems(0, this->plot->seriesList().at(0)->dataProxy()->itemCount());
}

void Plot3D::BuildTargetTrajectory(QVector3D start, QVector3D end){
    QVector3D d_vec = end - start;
    float dots_amount = d_vec.length() / CM_PER_DOT;
    QVector3D step_vector(d_vec / dots_amount);

    float iter;
    float iter_step;

    if ((std::abs(d_vec.x()) >= std::abs(d_vec.y())) && (std::abs(d_vec.x()) >= std::abs(d_vec.z()))){
        iter = std::abs(d_vec.x());
        iter_step = std::abs(step_vector.x());
    }
    else if (std::abs(d_vec.y()) >= std::abs(d_vec.z())){
        iter = std::abs(d_vec.y());
        iter_step = std::abs(step_vector.y());
    }
    else{
        iter = std::abs(d_vec.z());
        iter_step = std::abs(step_vector.z());
    }

    for ( ; iter > 0; start += step_vector, iter -= iter_step){
        this->AddTargetPoint(start);
    }
}

QVector3D Plot3D::DirectTransform(VertexCartesian Drives, QVector<float> length){
    QVector<QVector<float>> M;
    M.resize(3);
    for (auto iter = M.begin(); iter != M.end(); iter++)
            iter->resize(3);
    QVector<float> V;
    V.resize(3);

    M[0][0] = 2 * (Drives.Vertex[1].x() - Drives.Vertex[0].x());
    M[0][1] = 2 * (Drives.Vertex[1].y() - Drives.Vertex[0].y());
    M[0][2] = 2 * (Drives.Vertex[1].z() - Drives.Vertex[0].z());
    M[1][0] = 2 * (Drives.Vertex[2].x() - Drives.Vertex[0].x());
    M[1][1] = 2 * (Drives.Vertex[2].y() - Drives.Vertex[0].y());
    M[1][2] = 2 * (Drives.Vertex[2].z() - Drives.Vertex[0].z());
    M[2][0] = 2 * (Drives.Vertex[2].x() - Drives.Vertex[1].x());
    M[2][1] = 2 * (Drives.Vertex[2].y() - Drives.Vertex[1].y());
    M[2][2] = 2 * (Drives.Vertex[2].z() - Drives.Vertex[1].z());
    qDebug() << M[0][0];
    qDebug() << M;
    QVector3D B;
    V[0] =
        std::pow(Drives.Vertex[0].x(), 2) - std::pow(Drives.Vertex[1].x(), 2) +
        std::pow(Drives.Vertex[0].y(), 2) - std::pow(Drives.Vertex[1].y(), 2) +
        std::pow(Drives.Vertex[0].z(), 2) - std::pow(Drives.Vertex[1].z(), 2) -
        std::pow(length[0], 2) + std::pow(length[1], 2);

    V[1] =
        std::pow(Drives.Vertex[0].x(), 2) - std::pow(Drives.Vertex[2].x(), 2) +
        std::pow(Drives.Vertex[0].y(), 2) - std::pow(Drives.Vertex[2].y(), 2) +
        std::pow(Drives.Vertex[0].z(), 2) - std::pow(Drives.Vertex[2].z(), 2) -
        std::pow(length[0], 2) + std::pow(length[2], 2);
    V[2] =
        std::pow(Drives.Vertex[1].x(), 2) - std::pow(Drives.Vertex[2].x(), 2) +
        std::pow(Drives.Vertex[1].y(), 2) - std::pow(Drives.Vertex[2].y(), 2) +
        std::pow(Drives.Vertex[1].z(), 2) - std::pow(Drives.Vertex[2].z(), 2) -
        std::pow(length[1], 2) + std::pow(length[2], 2);
    qDebug() << V;
    float det =
            M[0][0]*M[1][1]*M[2][2] + M[0][1]*M[1][2]*M[2][0] + M[0][2]*M[1][0]*M[2][1] -
            M[0][2]*M[1][1]*M[2][0] - M[0][0]*M[1][2]*M[2][1] - M[0][1]*M[1][0]*M[2][2];
    qDebug() << "DET: " << det;
    float detX =
            V[0]*M[1][1]*M[2][2] + M[0][1]*M[1][2]*V[2] + M[0][2]*V[1]*M[2][1] -
            M[0][2]*M[1][1]*V[2] - M[0][1]*V[1]*M[2][2] - V[0]*M[1][2]*M[2][1];
    qDebug() << "DET X: " << detX;
    float detY =
            M[0][0]*V[1]*M[2][2] + M[0][2]*M[1][0]*V[2] + V[0]*M[1][2]*M[2][0] -
            M[0][2]*V[1]*M[2][0] - M[0][0]*M[1][2]*V[2] - V[0]*M[1][0]*M[2][2];
    qDebug() << "DET Y: " << detY;
    float detZ =
            M[0][0]*M[1][1]*V[2] + M[0][1]*V[1]*M[2][0] + V[0]*M[1][0]*M[2][1] -
            V[0]*M[1][1]*M[2][0] - M[0][0]*V[1]*M[2][1] - M[0][1]*M[1][0]*V[2];
    qDebug() << "DET Z: " << detZ;
    QVector3D res;
    res.setX(detX / det); res.setY(detY / det); res.setZ(detZ / det);
    return res;
}

QVector<float> Plot3D::InverseTransform(VertexCartesian Drives, QVector3D Object){
    QVector<float> res;
    res.push_back(std::sqrt(std::pow(Object.x() - Drives.Vertex[0].x(), 2) + std::pow(Object.y() - Drives.Vertex[0].y(), 2) + std::pow(Object.z() - Drives.Vertex[0].z(), 2)));
    res.push_back(std::sqrt(std::pow(Object.x() - Drives.Vertex[1].x(), 2) + std::pow(Object.y() - Drives.Vertex[1].y(), 2) + std::pow(Object.z() - Drives.Vertex[1].z(), 2)));
    res.push_back(std::sqrt(std::pow(Object.x() - Drives.Vertex[2].x(), 2) + std::pow(Object.y() - Drives.Vertex[2].y(), 2) + std::pow(Object.z() - Drives.Vertex[2].z(), 2)));
    res.push_back(std::sqrt(std::pow(Object.x() - Drives.Vertex[3].x(), 2) + std::pow(Object.y() - Drives.Vertex[3].y(), 2) + std::pow(Object.z() - Drives.Vertex[3].z(), 2)));
    return res;
}
