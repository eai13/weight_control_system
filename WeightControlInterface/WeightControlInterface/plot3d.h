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
#include <QTime>
#include <QTimer>

#define DEFAULT_MAP_HEIGHT  ((float)(2.60))
#define DEFAULT_MAP_WIDTH   ((float)(2.60))
#define DEFAULT_MAP_LENGTH  ((float)(3.33))

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

    QVector3D   object_real_position;
    QVector3D   object_setpoint_position;
    float       last_experiment_time;
    QVector3D   last_required_position;
    QVector3D   curr_required_position;

    struct ExperimentItem{
        QVector3D   profile;
        QVector3D   real;
        float       time;
        ExperimentItem(QVector3D p, QVector3D r, float t){
            this->profile = p; this->real = r; this->time = t;
        }
    };
    struct ProfileItem{
        QVector3D   profile;
        float       time;
        ProfileItem(QVector3D p, float t){
            this->profile = p; this->time = t;
        }
    };

    QList<ProfileItem>      profile;
    QList<ExperimentItem>   last_experiment_data;
    QTime *                 experiment_time = nullptr;
    QTimer *                control_timer = nullptr;
    void PushExperimentData(QVector3D profile_pt, QVector3D real_pt){
        if (this->experiment_time == nullptr) return;
        this->last_experiment_data.push_back(ExperimentItem(profile_pt, real_pt, (float)(this->experiment_time->elapsed()) / (float)(1000)));
    }
    void ClearExperimentData(void){
        if (this->last_experiment_data.isEmpty()) return;
        this->last_experiment_data.clear();
    }

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
    void slAbortTrajectory(void);
    void slPauseTrajectory(void);

    void slSaveReal(void);
    void slSaveTarget(void);
    void slUploadTarget(uint8_t format);

    void slReceiveRealLength(float length1, float length2, float length3, float length4){
        QVector<float> len;
        len.push_back(length1); len.push_back(length2); len.push_back(length3); len.push_back(length4);
        this->object_real_position = this->DirectTransform(len);
        qDebug() << "OBJECT REAL POSITION " << this->object_real_position;
        this->plot->seriesList().at(1)->dataProxy()->addItem(QtDataVisualization::QScatterDataItem(this->object_real_position));
        this->plot->seriesList().at(1)->dataProxy()->removeItems(0, 1);
    }
    void slReceiveSetpointLength(float length1, float length2, float length3, float length4){
        QVector<float> len;
        len.push_back(length1); len.push_back(length2); len.push_back(length3); len.push_back(length4);
        this->object_setpoint_position = this->DirectTransform(len);
    }

    void slReceiveObjectStep(QVector3D step);

private slots:

    void slControlCallback(void);

signals:
    void siSendTargetLength(float length1, float length2, float length3, float length4);
    void siEnableStart(uint8_t state);
    void siEnableStop(uint8_t state);
    void siEnableAbort(uint8_t state);
    void siEnableFullscreen(void);

};

#endif // PLOT3D_H
