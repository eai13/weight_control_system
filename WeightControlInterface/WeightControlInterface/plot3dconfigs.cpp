#include "plot3dconfigs.h"
#include "ui_plot3dconfigs.h"

#include <iostream>

Plot3DConfigs::Plot3DConfigs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Plot3DConfigs)
{
    ui->setupUi(this);

    ui->lineEdit_targetX->setValidator(new QDoubleValidator(-1000, 1000, 3));
    ui->lineEdit_targetY->setValidator(new QDoubleValidator(-1000, 1000, 3));
    ui->lineEdit_targetZ->setValidator(new QDoubleValidator(-1000, 1000, 3));

    ui->lineEdit_targetX->setText("0,000");
    ui->lineEdit_targetY->setText("0,000");
    ui->lineEdit_targetZ->setText("0,000");

    ui->progressBar->setValue(0);

    connect(ui->pushButton_3dfull, &QPushButton::released, this, &Plot3DConfigs::slFullScreen);
    connect(ui->pushButton_3dpause, &QPushButton::released, this, &Plot3DConfigs::slPauseTrajectory);
    connect(ui->pushButton_3dstart, &QPushButton::released, this, &Plot3DConfigs::slStartTrajectory);
    connect(ui->pushButton_3dstop, &QPushButton::released, this, &Plot3DConfigs::slStopTrajectory);
    connect(ui->pushButton_target_add, &QPushButton::released, this, &Plot3DConfigs::slTargetAdd);
    connect(ui->pushButton_target_clear, &QPushButton::released, this, &Plot3DConfigs::slTargetClear);
    connect(ui->pushButton_target_remove, &QPushButton::released, this, &Plot3DConfigs::slTargetRemove);
    connect(ui->pushButton_savetarget, &QPushButton::released, this, &Plot3DConfigs::slSaveTarget);
    connect(ui->pushButton_savereal, &QPushButton::released, this, &Plot3DConfigs::slSaveReal);
    connect(ui->pushButton_loadtarget, &QPushButton::released, this, &Plot3DConfigs::slUploadTarget);

    connect(ui->pushButton_jogXplusSMALL,   &QPushButton::released, this, &Plot3DConfigs::slJogXPlusSMALL);
    connect(ui->pushButton_jogXplusBIG,     &QPushButton::released, this, &Plot3DConfigs::slJogXPlusBIG);
    connect(ui->pushButton_jogXminusSMALL,  &QPushButton::released, this, &Plot3DConfigs::slJogXMinusSMALL);
    connect(ui->pushButton_jogXminusBIG,    &QPushButton::released, this, &Plot3DConfigs::slJogXMinusBIG);

    connect(ui->pushButton_jogYplusSMALL,   &QPushButton::released, this, &Plot3DConfigs::slJogYPlusSMALL);
    connect(ui->pushButton_jogYplusBIG,     &QPushButton::released, this, &Plot3DConfigs::slJogYPlusBIG);
    connect(ui->pushButton_jogYminusSMALL,  &QPushButton::released, this, &Plot3DConfigs::slJogYMinusSMALL);
    connect(ui->pushButton_jogYminusBIG,    &QPushButton::released, this, &Plot3DConfigs::slJogYMinusBIG);

    connect(ui->pushButton_jogZplusSMALL,   &QPushButton::released, this, &Plot3DConfigs::slJogZPlusSMALL);
    connect(ui->pushButton_jogZplusBIG,     &QPushButton::released, this, &Plot3DConfigs::slJogZPlusBIG);
    connect(ui->pushButton_jogZminusSMALL,  &QPushButton::released, this, &Plot3DConfigs::slJogZMinusSMALL);
    connect(ui->pushButton_jogZminusBIG,    &QPushButton::released, this, &Plot3DConfigs::slJogZMinusBIG);
}

Plot3DConfigs::~Plot3DConfigs()
{
    delete ui;
}


void Plot3DConfigs::slUpdateBar(int perc){
    Q_UNUSED(perc);
}

void Plot3DConfigs::Reset(void){
    ui->lineEdit_targetX->setText("");
    ui->lineEdit_targetY->setText("");
    ui->lineEdit_targetZ->setText("");

    ui->progressBar->setValue(0);
}

void Plot3DConfigs::slUploadTarget(void){
    emit siUploadTarget(ui->comboBox_targetfiletype->currentIndex());
}

void Plot3DConfigs::slTargetAdd(void){
    QString x_str = ui->lineEdit_targetX->text();
    for (auto iter = x_str.begin(); iter != x_str.end(); iter++) if (*iter == ',') *iter = '.';
    QString y_str = ui->lineEdit_targetY->text();
    for (auto iter = y_str.begin(); iter != y_str.end(); iter++) if (*iter == ',') *iter = '.';
    QString z_str = ui->lineEdit_targetZ->text();
    for (auto iter = z_str.begin(); iter != z_str.end(); iter++) if (*iter == ',') *iter = '.';
    QVector3D res(x_str.toFloat(), y_str.toFloat(), z_str.toFloat());
    emit siTargetAdd(res);
}

void Plot3DConfigs::slEnableAbort(uint8_t state){
    ui->pushButton_3dstop->setEnabled(state);
}
void Plot3DConfigs::slEnableStart(uint8_t state){
    ui->pushButton_3dstart->setEnabled(state);
}
void Plot3DConfigs::slEnableStop(uint8_t state){
    ui->pushButton_3dpause->setEnabled(state);
}

void Plot3DConfigs::slEnableFullscreen(void){
    ui->pushButton_3dfull->setEnabled(true);
}
void Plot3DConfigs::slFullScreen(void){
    ui->pushButton_3dfull->setEnabled(false);
    emit siFullscreen();
}
