#include "plot2d.h"

Plot2D::Plot2D(QCustomPlot * plot_h, QRadioButton * rb_turn_h, QRadioButton * rb_rads_h, QDial * dial_h, QLineEdit * lineedit_h){
    this->plot = plot_h;
    this->rb_turn = rb_turn_h;
    this->rb_rads = rb_rads_h;
    this->lineedit = lineedit_h;
    this->dial = dial_h;

    this->plot->clearItems();
    this->plot->xAxis->setLabel("Time, s");
    this->plot->yAxis->setLabel("Parameter");
    this->plot->xAxis->setRange(0, 10);
    this->plot->yAxis->setRange(-1, 1);
    this->plot->legend->setVisible(true);
    this->plot->setInteraction(QCP::iSelectPlottables, true);

    this->plot_menu.clear();
    QAction * p_act = nullptr;
    p_act = this->plot_menu.addAction("is Active");
    p_act->setObjectName("isactive");
    p_act->setCheckable(true);
    p_act->setChecked(true);

    this->plot_menu.addSeparator();

    p_act = this->plot_menu.addAction("Auto Rescale");
    p_act->setObjectName("Autorescale");
    p_act->setCheckable(true);

    this->plot_menu.addSeparator();

        QMenu * p_menu = this->plot_menu.addMenu("Active Registers");
        p_menu->addAction("Position Setpoint")->setObjectName("1");
        p_menu->addAction("Position")->setObjectName("2");
        p_menu->addAction("Speed Setpoint")->setObjectName("10");
        p_menu->addAction("Speed")->setObjectName("11");
        p_menu->addAction("Current Setpoint")->setObjectName("19");
        p_menu->addAction("Current")->setObjectName("20");
        p_menu->addAction("Motor Voltage")->setObjectName("28");

        for (auto iter = p_menu->actions().begin(); iter != p_menu->actions().end(); iter++){
            (*iter)->setCheckable(true);
            connect((*iter), &QAction::triggered, this, &Plot2D::slSetActiveRegister);
        }

        this->plot->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this->plot, &QCustomPlot::customContextMenuRequested, this, &Plot2D::slShowContextMenu);

    this->pencolor_buffer.clear();
    this->pencolor_buffer.push_back(QColor(255, 0, 0));
    this->pencolor_buffer.push_back(QColor(0, 255, 0));
    this->pencolor_buffer.push_back(QColor(0, 0, 255));
    this->pencolor_buffer.push_back(QColor(255, 0, 255));
    this->pencolor_buffer.push_back(QColor(255, 255, 0));
    this->pencolor_buffer.push_back(QColor(0, 255, 255));
    this->pencolor_buffer.push_back(QColor(0, 0, 0));

    this->lineedit->setValidator(new QDoubleValidator(-1000, 1000, 3));

//    connect(this->lineedit, &QLineEdit::returnPressed, this, )
    connect(this->rb_turn, &QRadioButton::clicked, this, &Plot2D::slSetTurns);
    connect(this->rb_rads, &QRadioButton::clicked, this, &Plot2D::slSetRads);
    connect(this->dial, &QDial::sliderMoved, this, &Plot2D::slProcessDial);
    connect(this->dial, &QDial::sliderReleased, this, &Plot2D::slSendPosFromDial);

    this->system_time = new QTime();
    this->system_time->start();
}

void Plot2D::slSetActiveRegister(bool state){
    uint32_t sender_id = sender()->objectName().toInt();

    if (state){
        for (auto iter = this->active_registers.begin(); iter != this->active_registers.end(); iter++)
            if ((*iter).reg == sender_id) return;
        QCPGraph * tmp = this->plot->addGraph();
        tmp->setPen(QPen(this->GrabColor()));
        this->active_registers.push_back(Register(sender_id, tmp));
        this->system_time->restart();
    }
    else{
        for (uint32_t iter = 0; iter < this->active_registers.size(); ){
            if (this->active_registers.at(iter).reg == sender_id){
                this->ReturnColor(this->active_registers.at(iter).plot_id->pen().color());
                this->plot->removeGraph(this->active_registers.at(iter).plot_id);
                this->active_registers.removeAt(iter);
                continue;
            }
            iter++;
        }
    }
}

void Plot2D::ResetDial(void){
    this->dial_old_value = 0;
    this->dial_counter = 0;
    this->dial->setValue(0);
    this->lineedit->setText("0,000");
    this->rb_rads->setChecked(true);
}

void Plot2D::slAddData(uint32_t reg, float value){
    if (this->plot_menu.findChild<QAction *>("isactive")->isChecked() == false) return;
    for (auto iter = this->active_registers.begin(); iter != this->active_registers.end(); iter++){
        if ((*iter).reg == reg){
            (*iter).plot_id->addData((float)this->system_time->elapsed() / 1000.0, value);
            if (this->plot_menu.findChild<QAction *>("Autorescale")->isChecked()) this->plot->rescaleAxes();
            return;
        }
    }
}

void Plot2D::slProcessDial(int data){
    if (std::abs(this->dial_old_value - data) > this->dial->maximum() / 2){
        if (this->dial_old_value - data > 0)
            this->dial_counter++;
        else
            this->dial_counter--;
    }
    this->dial_old_value = data;
    if (this->rb_turn->isChecked())
        this->lineedit->setText(QString::fromStdString(std::to_string(
            float(data + this->dial_counter * (this->dial->maximum() + 1)) / (this->dial->maximum() + 1))));
    else
        this->lineedit->setText(QString::fromStdString(std::to_string(
            float(data + this->dial_counter * (this->dial->maximum() + 1)) / (this->dial->maximum() + 1) * 6.28)));
}

void Plot2D::slSendPosFromDial(void){
    QString str = this->lineedit->text();
    for (auto iter = str.begin(); iter != str.end(); iter++)
        if (*iter == ',') *iter = '.';
    if (this->rb_rads->isChecked())
        emit siSendPos(str.toFloat());
    else
        emit siSendPos(str.toFloat() * 6.28);
}

void Plot2D::slSetTurns(void){
    float new_val = (float)(this->dial_counter * (this->dial->maximum() + 1) + this->dial->value()) / (float)(this->dial->maximum());
    this->lineedit->setText(QString::fromStdString(std::to_string(new_val)));
}

void Plot2D::slSetRads(void){
    float new_val = (float)(this->dial_counter * (this->dial->maximum() + 1) + this->dial->value()) / (float)(this->dial->maximum()) * 6.28;
    this->lineedit->setText(QString::fromStdString(std::to_string(new_val)));
}

void Plot2D::slShowContextMenu(const QPoint & pos){
    this->plot_menu.exec(QCursor::pos());
}
