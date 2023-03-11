#include "plot3d.h"
#include <QtCore/qglobal.h>
#include <iostream>
#include <QFileDialog>
#include <QString>

Plot3D::Plot3D(QGroupBox *parent) : QObject(parent){

    this->map_height = DEFAULT_MAP_HEIGHT;
    this->map_width = DEFAULT_MAP_WIDTH;
    this->map_length = DEFAULT_MAP_LENGTH;

    this->plot = new QtDataVisualization::Q3DScatter();
    this->plot_widget = QWidget::createWindowContainer(this->plot);
    this->group_box_parent = parent;
    this->group_box_parent->layout()->addWidget(this->plot_widget);
    this->plot_widget->show();

    this->plot->axisX()->setAutoAdjustRange(false);
    this->plot->axisX()->setRange(0, this->map_width);
    this->plot->axisY()->setAutoAdjustRange(false);
    this->plot->axisY()->setRange(0, this->map_length);
    this->plot->axisZ()->setAutoAdjustRange(false);
    this->plot->axisZ()->setRange(0, this->map_height);

    this->plot->activeTheme()->setType(QtDataVisualization::Q3DTheme::ThemePrimaryColors);
    this->plot->activeTheme()->setFont(QFont("Sans Serif", 50, QFont::Weight::Normal));
    this->plot->setShadowQuality(QtDataVisualization::QAbstract3DGraph::ShadowQualityNone);
    this->plot->setTitle("Object Trajectory");

    QtDataVisualization::QScatterDataProxy * target_proxy = new QtDataVisualization::QScatterDataProxy;
    QtDataVisualization::QScatterDataProxy * real_proxy = new QtDataVisualization::QScatterDataProxy;
    QtDataVisualization::QScatterDataProxy * real_traj_proxy = new QtDataVisualization::QScatterDataProxy;
    QtDataVisualization::QScatter3DSeries * target_series = new QtDataVisualization::QScatter3DSeries(target_proxy);
    QtDataVisualization::QScatter3DSeries * real_series = new QtDataVisualization::QScatter3DSeries(real_proxy);
    QtDataVisualization::QScatter3DSeries * real_traj_series = new QtDataVisualization::QScatter3DSeries(real_traj_proxy);

    target_series->setItemSize(0.1);
    target_series->setItemLabelFormat(QStringLiteral("Target Trajectory: @xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    target_series->setMeshSmooth(true);
    target_series->setBaseColor(QColor(0, 0, 255));
    this->plot->addSeries(target_series);

    real_series->setItemLabelFormat(QStringLiteral("Real Object: @xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    real_series->setMeshSmooth(true);
    real_series->setBaseColor(QColor(255, 0, 0));
    this->plot->addSeries(real_series);
    this->plot->seriesList().at(1)->dataProxy()->addItem(QtDataVisualization::QScatterDataItem(QVector3D(1, 1, 1)));

    real_traj_series->setItemSize(0.15);
    real_traj_series->setItemLabelFormat(QStringLiteral("Real Trajectory: @xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    real_traj_series->setMeshSmooth(true);
    real_traj_series->setBaseColor(QColor(255, 0, 0));
    this->plot->addSeries(real_traj_series);

    this->plot->axisX()->setTitle("X");
    this->plot->axisX()->setTitleVisible(true);
    this->plot->axisY()->setTitle("Y");
    this->plot->axisY()->setTitleVisible(true);
    this->plot->axisZ()->setTitle("Z");
    this->plot->axisZ()->setTitleVisible(true);

    this->control_timer = new QTimer;
    connect(this->control_timer, &QTimer::timeout, this, &Plot3D::slControlCallback);
    this->experiment_time = new QTime;
}

void Plot3D::AddRealPoint(double x, double y, double z){
    this->plot->seriesList().at(2)->dataProxy()->addItem(QtDataVisualization::QScatterDataItem(QVector3D(x, y, z)));
}
void Plot3D::AddRealPoint(QVector3D pt){
    this->plot->seriesList().at(2)->dataProxy()->addItem(QtDataVisualization::QScatterDataItem(pt));
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
    this->plot->seriesList().at(2)->dataProxy()->removeItems(0, this->plot->seriesList().at(1)->dataProxy()->itemCount());
}

void Plot3D::ClearTarget(void){
    this->plot->seriesList().at(0)->dataProxy()->removeItems(0, this->plot->seriesList().at(0)->dataProxy()->itemCount());
}

void Plot3D::BuildTargetTrajectory(QVector3D start, QVector3D end){
    QVector3D d_vec = end - start;
    float dots_amount = d_vec.length() / M_PER_DOT;
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

QVector3D Plot3D::DirectTransform(QVector<float> length){
    float resX = (std::pow(this->map_width, 2) - std::pow(length[3], 2) + std::pow(length[2], 2)) / (2 * this->map_width);
    float resY = (std::pow(this->map_length, 2) - std::pow(length[1], 2) + std::pow(length[2], 2)) / (2 * this->map_length);
    qDebug() << "CHECK " << std::pow(length[0], 2) - std::pow(resX - this->map_width, 2) - std::pow(resY - this->map_length, 2);
    float resZ = this->map_height - std::sqrt(std::pow(length[0], 2) - std::pow(resX - this->map_width, 2) - std::pow(resY - this->map_length, 2));

    QVector3D res;
    res.setX(resX); res.setY(resY); res.setZ(resZ);
    return res;
}

QVector<float> Plot3D::InverseTransform(QVector3D object){
    QVector<float> res;
    res.push_back(std::sqrt(std::pow(object.x() - this->map_width, 2) + std::pow(object.y() - this->map_length, 2) + std::pow(object.z() - this->map_height, 2)));
    res.push_back(std::sqrt(std::pow(object.x(), 2) + std::pow(object.y() - this->map_length, 2) + std::pow(object.z() - this->map_height, 2)));
    res.push_back(std::sqrt(std::pow(object.x(), 2) + std::pow(object.y(), 2) + std::pow(object.z() - this->map_height, 2)));
    res.push_back(std::sqrt(std::pow(object.x() - this->map_width, 2) + std::pow(object.y(), 2) + std::pow(object.z() - this->map_height, 2)));
    return res;
}

float Plot3D::AngleFromLength(float length){
    return length;
}

float Plot3D::LengthFromAngle(float angle){
    return angle;
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
}
void Plot3D::slFullscreenClosed(void){
    this->group_box_parent->layout()->addWidget(this->plot_widget);
    this->plot_widget->show();
    emit this->siEnableFullscreen();
}

void Plot3D::slTargetAdd(QVector3D data){
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
    this->ClearTarget();
}
void Plot3D::slTargetRemove(void){
    this->RemoveLastTarget();
}

void Plot3D::slStartTrajectory(void){
    if (this->plot->seriesList().at(0)->dataProxy()->itemCount() == 0) return;
    this->profile.clear();
    float time = 0;
    for (int32_t iter = 0; iter < this->plot->seriesList().at(0)->dataProxy()->itemCount(); iter++){
        float x = this->plot->seriesList().at(0)->dataProxy()->itemAt(iter)->x();
        float y = this->plot->seriesList().at(0)->dataProxy()->itemAt(iter)->y();
        float z = this->plot->seriesList().at(0)->dataProxy()->itemAt(iter)->z();
        this->profile.push_back(ProfileItem(QVector3D(x, y, z), time));
        time += 0.5;
        qDebug() << "X: " << x << " Y: " << y << " Z: " << z << " TIME: " << time;
        QVector<float> tmp = this->InverseTransform(QVector3D(x, y, z));
        qDebug() << "LEN 1 : " << tmp[0] << "LEN 2 : " << tmp[1] << "LEN 3 : " << tmp[2] << "LEN 4 : " << tmp[3];
    }

    if (this->control_timer == nullptr) this->control_timer = new QTimer;
    this->control_timer->start(60);

    if (this->experiment_time == nullptr) this->experiment_time = new QTime;
    this->experiment_time->restart();

    emit this->siEnableStart(0);
    emit this->siEnableStop(1);
    emit this->siEnableAbort(1);
    qDebug() << "Plot3D Start Trajectory";
}
void Plot3D::slAbortTrajectory(void){
    if (this->control_timer != nullptr)
        if (this->control_timer->isActive()) this->control_timer->stop();

    emit this->siEnableStart(1);
    emit this->siEnableStop(0);
    emit this->siEnableAbort(0);
    qDebug() << "Plot3D Stop Trajectory";
}
void Plot3D::slPauseTrajectory(void){
    this->last_experiment_time = (float)(this->experiment_time->elapsed()) / (float)(1000);
    if (this->control_timer != nullptr)
        if (this->control_timer->isActive()) this->control_timer->stop();

    emit this->siEnableStart(1);
    emit this->siEnableStop(0);
    emit this->siEnableAbort(1);
    qDebug() << "Plot3D Pause Trajectory";
}

void Plot3D::slControlCallback(void){
    if (this->profile.isEmpty()){
        qDebug() << "TRAJECTORY FINISHED";
        return;
    }
    QVector3D current_pos = this->object_real_position;
    float current_time = (float)(this->experiment_time->elapsed()) / (float)(1000);
    ProfileItem pt = this->profile.front();
    if (pt.time < current_time){
        this->profile.pop_front();
        QVector<float> pos = this->InverseTransform(pt.profile);
        qDebug() << "REQUIRED POSITIONS: " << pos;
        emit this->siSendTargetLength(pos[0], pos[1], pos[2], pos[3]);
    }
    this->AddRealPoint(current_pos);
    this->PushExperimentData(pt.profile, current_pos);
}

void Plot3D::slSaveReal(void){
    qDebug() << "Plot3D Save Real";
    QString filter;
    QString fname = QFileDialog::getSaveFileName(nullptr, "Save Target Trajectory", QDir::currentPath(), "CSV (*.csv)", &filter);
    QFile file(fname + ".csv");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream file_stream(&file);
        file_stream << "X;Y;Z\n";
        for (int32_t iter = 0; iter < this->plot->seriesList().at(2)->dataProxy()->itemCount(); iter++){
            float x = this->plot->seriesList().at(2)->dataProxy()->itemAt(iter)->x();
            float y = this->plot->seriesList().at(2)->dataProxy()->itemAt(iter)->y();
            float z = this->plot->seriesList().at(2)->dataProxy()->itemAt(iter)->z();
            file_stream << QString::asprintf("%.2f", x) << ";" <<
                           QString::asprintf("%.2f", y) << ";" <<
                           QString::asprintf("%.2f", z) << "\n";
        }
        file.close();
    }
}
void Plot3D::slSaveTarget(void){
    qDebug() << "Plot3D Save Target";
    QString filter;
    QString fname = QFileDialog::getSaveFileName(nullptr, "Save Target Trajectory", QDir::currentPath(), "CSV (*.csv)", &filter);
    QFile file(fname + ".csv");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream file_stream(&file);
        file_stream << "X;Y;Z\n";
        for (int32_t iter = 0; iter < this->plot->seriesList().at(0)->dataProxy()->itemCount(); iter++){
            float x = this->plot->seriesList().at(0)->dataProxy()->itemAt(iter)->x();
            float y = this->plot->seriesList().at(0)->dataProxy()->itemAt(iter)->y();
            float z = this->plot->seriesList().at(0)->dataProxy()->itemAt(iter)->z();
            file_stream << QString::asprintf("%.2f", x) << ";" <<
                           QString::asprintf("%.2f", y) << ";" <<
                           QString::asprintf("%.2f", z) << "\n";
        }
        file.close();
    }
}
void Plot3D::slUploadTarget(uint8_t format){
    qDebug() << "Plot3D Upload Target";
    QString HeaderString;
    QStringList CurrentLine;
    QString filter;
    QString fname = QFileDialog::getOpenFileName(nullptr, "Upload Target Trajectory", QDir::currentPath(), "CSV (*.csv)", &filter);
    QFile file(fname);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream file_stream(&file);
        file_stream >> HeaderString;
        if (HeaderString == "X;Y;Z"){
            file_stream >> HeaderString;
            while(HeaderString.size() != 0){
                CurrentLine = HeaderString.split(";");
                if (CurrentLine.size() == 3)
                    this->slTargetAdd(QVector3D(CurrentLine[0].toDouble(), CurrentLine[1].toDouble(), CurrentLine[2].toDouble()));
                else
                    break;
                file_stream >> HeaderString;
            }
        }
        file.close();
    }
}

void Plot3D::slReceiveObjectStep(QVector3D step){
    QVector<float> len = this->InverseTransform(this->object_setpoint_position + step);
    emit this->siSendTargetLength(len[0], len[1], len[2], len[3]);
}
