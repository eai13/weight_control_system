#include "plot3d.h"
#include <QtCore/qglobal.h>

#define POLYGON_Y_SIZE  100
#define POLYGON_X_SIZE  100
#define POLYGON_Z_SIZE  100

#define CM_PER_DOT      2

Plot3D::Plot3D(QGroupBox * parent){
    this->plot = new QtDataVisualization::Q3DScatter();
    QWidget * plot_widget = QWidget::createWindowContainer(this->plot);
    parent->layout()->addWidget(plot_widget);
    plot_widget->show();

    QObject::connect(parent, &QGroupBox::clicked, this, &Plot3D::slClicked);

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
//        qDebug() << start;
        this->AddTargetPoint(start);
    }
}

void Plot3D::slClicked(void){
    qDebug() << "Clicked";
}
