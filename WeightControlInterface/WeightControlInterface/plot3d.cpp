#include "plot3d.h"
#include <QtCore/qglobal.h>
#include <iostream>

Plot3D::Plot3D(QGroupBox *parent) : QObject(parent){
    this->AngleFromLength(5);

    this->plot = new QtDataVisualization::Q3DScatter();
    this->plot_widget = QWidget::createWindowContainer(this->plot);
    this->group_box_parent = parent;
    this->group_box_parent->layout()->addWidget(this->plot_widget);
    this->plot_widget->show();

    this->drive_vertex = new VertexCartesian(QVector3D(0, 0, POLYGON_Z_SIZE),
                                             QVector3D(POLYGON_X_SIZE, 0, POLYGON_Z_SIZE),
                                             QVector3D(0, POLYGON_Y_SIZE, POLYGON_Z_SIZE),
                                             QVector3D(POLYGON_X_SIZE, POLYGON_Y_SIZE, POLYGON_Z_SIZE));

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
void Plot3D::RemoveLastTarget(void){
    if (this->plot->seriesList().at(0)->dataProxy()->itemCount())
        this->plot->seriesList().at(0)->dataProxy()->removeItems(this->plot->seriesList().at(0)->dataProxy()->itemCount() - 1, 1);
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
    float resX = (std::pow(length[0], 2) - std::pow(length[1], 2) + std::pow(Drives.Vertex[1].x(), 2)) / (2 * Drives.Vertex[1].x());
    float resY = (std::pow(length[0], 2) - std::pow(length[2], 2) + std::pow(Drives.Vertex[2].y(), 2)) / (2 * Drives.Vertex[2].y());
    float resZ = Drives.Vertex[0].z() - std::sqrt(std::pow(length[0], 2) - std::pow(resX, 2) - std::pow(resY, 2));

    QVector3D res;
    res.setX(resX); res.setY(resY); res.setZ(resZ);
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

float Plot3D::AngleFromLength(float length){
    float a = 1;
    float b = (2 * std::pow(DEFAULT_DIAL_RADIUS, 3) * std::pow(PI, 3) - 6 * PI * length * std::pow(ROPE_THICK, 2)) / std::pow(ROPE_THICK, 3);
    float c = std::pow(DEFAULT_DIAL_RADIUS, 6) * std::pow(PI, 6) / std::pow(ROPE_THICK, 6);

    float D = std::pow(b, 2) - 4 * a * c;
    std::cout << D;
    return 0;
}

float Plot3D::LengthFromAngle(float angle){
    float length = std::pow(angle, 2) * DEFAULT_DIAL_RADIUS / 2;
    length += std::pow(angle, 3) * ROPE_THICK / PI / 6;
    return length;
}

void Plot3D::slFullscreen(void){
    QPoint position = this->plot_widget->mapToGlobal(this->plot_widget->pos());
    QSize size = this->plot_widget->size();

    this->fullscreen = new QMainWindow;
    this->fullscreen->setWindowTitle("3D Plot");
    this->fullscreen->setAttribute(Qt::WA_DeleteOnClose);
    this->fullscreen->setWindowFlags(this->fullscreen->windowFlags() | Qt::WindowStaysOnTopHint);
    this->fullscreen->setCentralWidget(this->plot_widget);
    this->fullscreen->setGeometry(position.x(), position.y(), size.width(), size.height());
    connect(this->fullscreen, &QMainWindow::destroyed, this, &Plot3D::slFullscreenClosed);
    this->fullscreen->show();
//    this->plot_widget->show();
}
void Plot3D::slFullscreenClosed(void){
    qDebug() << "Fullscreen Closed";
    this->group_box_parent->layout()->addWidget(this->plot_widget);
    this->plot_widget->show();
}

void Plot3D::slTargetAdd(QVector3D data){
    qDebug() << "Plot3D Target Add";
    QVector3D start;
    if (this->plot->seriesList().at(0)->dataProxy()->itemCount()){
        start.setX(this->plot->seriesList().at(0)->dataProxy()->itemAt(this->plot->seriesList().at(0)->dataProxy()->itemCount() - 1)->x());
        start.setY(this->plot->seriesList().at(0)->dataProxy()->itemAt(this->plot->seriesList().at(0)->dataProxy()->itemCount() - 1)->y());
        start.setZ(this->plot->seriesList().at(0)->dataProxy()->itemAt(this->plot->seriesList().at(0)->dataProxy()->itemCount() - 1)->z());
    }
    else{
        start.setX(this->plot->seriesList().at(1)->dataProxy()->itemAt(this->plot->seriesList().at(1)->dataProxy()->itemCount() - 1)->x());
        start.setY(this->plot->seriesList().at(1)->dataProxy()->itemAt(this->plot->seriesList().at(1)->dataProxy()->itemCount() - 1)->y());
        start.setZ(this->plot->seriesList().at(1)->dataProxy()->itemAt(this->plot->seriesList().at(1)->dataProxy()->itemCount() - 1)->z());
    }
    this->BuildTargetTrajectory(start, data);
}
void Plot3D::slTargetClear(void){
    qDebug() << "Plot3D Target Clear";
    this->ClearTarget();
}
void Plot3D::slTargetRemove(void){
    qDebug() << "Plot3D Target Remove";
    this->RemoveLastTarget();
}

void Plot3D::slStartTrajectory(void){
    qDebug() << "Plot3D Start Trajectory";
}
void Plot3D::slStopTrajectory(void){
    qDebug() << "Plot3D Stop Trajectory";
}
void Plot3D::slPauseTrajectory(void){
    qDebug() << "Plot3D Pause Trajectory";
}

void Plot3D::slSaveReal(void){
    qDebug() << "Plot3D Save Real";
}
void Plot3D::slSaveTarget(void){
    qDebug() << "Plot3D Save Target";
}
void Plot3D::slUploadTarget(uint8_t format){
    qDebug() << "Plot3D Upload Target " << format;
}
