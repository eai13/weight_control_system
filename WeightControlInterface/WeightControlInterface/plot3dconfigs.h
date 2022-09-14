#ifndef PLOT3DCONFIGS_H
#define PLOT3DCONFIGS_H

#include <QVector3D>

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

private:

public slots:
    void slUpdateBar(int perc);

private slots:


signals:
    void siTargetAdd(QVector3D data);


};

#endif // PLOT3DCONFIGS_H
