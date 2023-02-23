#include "plot2d.h"
#include <QWidget>
#include <QBoxLayout>

Plot2D::Plot2D(QString title, QCustomPlot * plot_h,
               QRadioButton * rb_turn_h, QRadioButton * rb_rads_h, QRadioButton * rb_length_h,
               QDial * dial_h,
               QLineEdit * lineedit_h,
               QPushButton * zerocalib, QPushButton * stop, QPushButton * setpos,
               QPushButton * less_small, QPushButton * less_much, QPushButton * more_small, QPushButton * more_much,
               double A_cal, double B_cal, double C_cal, double MIN_cal, double MAX_cal){
    this->plot_parent = plot_h->parentWidget();
    this->plot = plot_h;
    this->rb_turn = rb_turn_h;
    this->rb_rads = rb_rads_h;
    this->rb_length = rb_length_h;
    this->lineedit = lineedit_h;
    this->dial = dial_h;
    this->pb_calibzero = zerocalib;
    this->pb_stop = stop;
    this->pb_setpos = setpos;
    this->pb_jogless_b = less_much;
    this->pb_jogless_s = less_small;
    this->pb_jogmore_b = more_much;
    this->pb_jogmore_s = more_small;

    this->trA_calib = A_cal;
    this->trB_calib = B_cal;
    this->trC_calib = C_cal;
    this->MAX_calib = MAX_cal;
    this->MIN_calib = MIN_cal;

    this->plot->clearItems();
    this->plot->plotLayout()->insertRow(0);
    this->plot->plotLayout()->addElement(0, 0, new QCPTextElement(this->plot, title, QFont("sans", 12, QFont::Bold)));
    this->plot->xAxis->setLabel("Time, s");
    this->plot->yAxis->setLabel("Parameter");
    this->plot->xAxis->setRange(0, 10);
    this->plot->yAxis->setRange(-1, 1);
    this->plot->legend->setVisible(true);
    this->plot->setInteraction(QCP::iSelectPlottables, true);
    this->plot->setInteraction(QCP::iRangeZoom, true);
    this->plot->setInteraction(QCP::iRangeDrag, true);

    this->plot_menu.clear();
    QAction * p_act = nullptr;
    p_act = this->plot_menu.addAction("is Active");
    p_act->setObjectName("isactive");
    p_act->setCheckable(true);
    p_act->setChecked(true);
    connect(p_act, &QAction::triggered, this, &Plot2D::slPlotActive);

    this->plot_menu.addSeparator();

    p_act = this->plot_menu.addAction("Auto Rescale");
    p_act->setObjectName("Autorescale");
    p_act->setCheckable(true);

    this->plot_menu.addAction("Rescale to Default", this, &Plot2D::slRescaleToDefault);

    this->plot_menu.addAction("Save Plot", this, &Plot2D::slSaveImage);
    this->plot_menu.addAction("Save Data", this, &Plot2D::slSaveData);

    this->plot_menu.addSeparator();

        QMenu * p_menu = this->plot_menu.addMenu("Active Registers");
        p_menu->setObjectName("registers");
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

    this->plot_menu.addSeparator();
    p_act = this->plot_menu.addAction("Fullscreen");
    p_act->setObjectName("fullscreen");
    p_act->setCheckable(true);
    p_act->setChecked(false);
    connect(p_act, &QAction::triggered, this, &Plot2D::slFullscreen);

    this->pencolor_buffer.clear();
    this->pencolor_buffer.push_back(QColor(255, 0, 0));
    this->pencolor_buffer.push_back(QColor(0, 255, 0));
    this->pencolor_buffer.push_back(QColor(0, 0, 255));
    this->pencolor_buffer.push_back(QColor(255, 0, 255));
    this->pencolor_buffer.push_back(QColor(255, 255, 0));
    this->pencolor_buffer.push_back(QColor(0, 255, 255));
    this->pencolor_buffer.push_back(QColor(0, 0, 0));

    this->lineedit->setValidator(new QDoubleValidator(-1000, 1000, 3));

    connect(this->lineedit,     &QLineEdit::returnPressed,  this, &Plot2D::slProcessEditLine);
    connect(this->dial,         &QDial::sliderMoved,        this, &Plot2D::slProcessDial);
    connect(this->dial,         &QDial::sliderReleased,     this, &Plot2D::slSendPosFromDial);
    connect(this->pb_calibzero, &QPushButton::released,     this, &Plot2D::slCalibrateZero);
    connect(this->pb_stop,      &QPushButton::released,     this, &Plot2D::slStopDrive);
    connect(this->pb_setpos,    &QPushButton::released,     this, &Plot2D::slSetPos);
    connect(this->pb_jogless_s, &QPushButton::released,     this, &Plot2D::slJogLessSmall);
    connect(this->pb_jogless_b, &QPushButton::released,     this, &Plot2D::slJogLessMuch);
    connect(this->pb_jogmore_s, &QPushButton::released,     this, &Plot2D::slJogMoreSmall);
    connect(this->pb_jogmore_b, &QPushButton::released,     this, &Plot2D::slJogMoreMuch);

    this->system_time = new QTime();
    this->system_time->start();
}

void Plot2D::slSetActiveRegister(bool state){
    uint32_t sender_id = sender()->objectName().toInt();

    if (state){
        for (auto iter = this->active_registers.begin(); iter != this->active_registers.end(); iter++)
            if ((*iter).reg == sender_id) return;
        QCPGraph * tmp = this->plot->addGraph();
        tmp->setName(this->register_names[sender_id]);
        tmp->setPen(QPen(this->GrabColor()));
        this->active_registers.push_back(Register(sender_id, tmp));
        this->system_time->restart();
    }
    else{
        for (int32_t iter = 0; iter < this->active_registers.size(); ){
            if (this->active_registers.at(iter).reg == sender_id){
                this->ReturnColor(this->active_registers.at(iter).plot_id->pen().color());
                this->plot->removeGraph(this->active_registers.at(iter).plot_id);
                this->active_registers.removeAt(iter);
                continue;
            }
            iter++;
        }
    }

    for (uint8_t iter = 0; iter < this->plot->graphCount(); iter++)
        this->plot->graph(iter)->data()->clear();

    this->plot->replot();
    this->system_time->restart();
}

void Plot2D::ResetPlot(void){
//    this->lineedit->setText("0,000");
    this->rb_rads->setChecked(true);
    this->dial->setValue(0);
    this->dial_counter = 0;
    this->dial_old_value = 0;
    this->plot->clearGraphs();
    this->plot->replot();
    this->plot_menu.findChild<QAction *>("isactive")->setChecked(true);
    this->plot_menu.findChild<QAction *>("Autorescale")->setChecked(false);
    QMenu * p_menu = this->plot_menu.findChild<QMenu *>("registers");
    for (auto iter = p_menu->actions().begin(); iter != p_menu->actions().end(); iter++){
        (*iter)->setChecked(false);
    }
    this->pencolor_buffer.clear();
    this->pencolor_buffer.push_back(QColor(255, 0, 0));
    this->pencolor_buffer.push_back(QColor(0, 255, 0));
    this->pencolor_buffer.push_back(QColor(0, 0, 255));
    this->pencolor_buffer.push_back(QColor(255, 0, 255));
    this->pencolor_buffer.push_back(QColor(255, 255, 0));
    this->pencolor_buffer.push_back(QColor(0, 255, 255));
    this->pencolor_buffer.push_back(QColor(0, 0, 0));

    this->active_registers.clear();
}

void Plot2D::slAddData(uint32_t reg, float value){
    if (this->plot_menu.findChild<QAction *>("isactive")->isChecked() == false) return;
    for (auto iter = this->active_registers.begin(); iter != this->active_registers.end(); iter++){
        if ((*iter).reg == reg){
            (*iter).plot_id->addData((float)this->system_time->elapsed() / 1000.0, value);
            if (this->plot_menu.findChild<QAction *>("Autorescale")->isChecked()) this->plot->rescaleAxes();
            this->plot->replot();
            return;
        }
    }
    return;
}

void Plot2D::slProcessEditLine(void){
    QString val_str = this->lineedit->text();
    for (auto iter = val_str.begin(); iter != val_str.end(); iter++)
        if ((*iter) == ',') (*iter) = '.';
    float val = val_str.toFloat();

    if (this->rb_rads->isChecked()){
        this->dial_counter = floor(val / 6.28);
        this->dial->setValue((int)(val / 6.28 * (this->dial->maximum() + 1)) % (this->dial->maximum() + 1));
        this->dial_old_value = this->dial->value();
        emit this->siSendPos(val);
    }
    else if (this->rb_turn->isChecked()){
        this->dial_counter = floor(val);
        this->dial->setValue((int)(val * (this->dial->maximum() + 1)) % (this->dial->maximum() + 1));
        this->dial_old_value = this->dial->value();
        emit this->siSendPos(val * 6.28);
    }
    else if (this->rb_length->isChecked()){
        float angle = this->GetAngleFromLength(val);
        this->dial_counter = floor(angle);
        this->dial->setValue((int)(angle * (this->dial->maximum() + 1)) % (this->dial->maximum() + 1));
        this->dial_old_value = this->dial->value();
        emit this->siSendPos(angle);
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
    if (this->rb_turn->isChecked()){
        this->lineedit->setText(QString::asprintf("%.4f", (
            float(data + this->dial_counter * (this->dial->maximum() + 1)) / (this->dial->maximum() + 1))));
    }
    else if (this->rb_rads->isChecked()){
        this->lineedit->setText(QString::asprintf("%.4f", (
            float(data + this->dial_counter * (this->dial->maximum() + 1)) / (this->dial->maximum() + 1) * 6.28)));
    }
    else if (this->rb_length->isChecked()){
        float angle = float(data + this->dial_counter * (this->dial->maximum() + 1)) / (this->dial->maximum() + 1) * 6.28;
        float length = this->GetLengthFromAngle(angle);
        this->lineedit->setText(QString::asprintf("%.4f", length));
    }
}

void Plot2D::slSendPosFromDial(void){
    float pos = float(this->dial->value() + this->dial_counter * (this->dial->maximum() + 1)) / (this->dial->maximum() + 1) * 6.28;
    this->siSendPos(pos);
}

void Plot2D::slShowContextMenu(const QPoint & pos){
    Q_UNUSED(pos);
    this->plot_menu.exec(QCursor::pos());
}

void Plot2D::slPlotActive(bool state){
    if (state){
        for (uint8_t iter = 0; iter < this->plot->graphCount(); iter++){
            this->plot->graph(iter)->data()->clear();
        }
        this->system_time->restart();
    }
}

void Plot2D::slSaveImage(void){
    QString filter;
    QString fname = QFileDialog::getSaveFileName(nullptr, "Save file", QDir::currentPath(), "PNG (*.png);;BMP (*.bmp);;JPG (*.jpg)", &filter);
    if (filter == "PNG (*.png)")
        this->plot->savePng(fname, 1920, 1080);
    else if (filter == "BMP (*.bmp)")
        this->plot->saveBmp(fname, 1920, 1080);
    else if (filter == "JPG (*.jpg)")
        this->plot->saveJpg(fname, 1920, 1080);
}

void Plot2D::slSaveData(void){
    QString filter;
    QString fname = QFileDialog::getSaveFileName(nullptr, "Save File", QDir::currentPath(), "CSV (*.csv)", &filter);
    QFile file(fname + ".csv");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream file_stream(&file);
        for (auto iter = this->active_registers.begin(); iter != this->active_registers.end(); iter++)
            file_stream << "Time;" << (*iter).plot_id->name() << ";";
        file_stream << "\n";

        QVector<uint64_t> vec_iter; vec_iter.resize(this->plot->graphCount());
        QVector<uint64_t> vec_end; vec_end.resize(this->plot->graphCount());
        for (uint8_t iter = 0; iter < this->plot->graphCount(); iter++){
            vec_iter[iter] = 0;
            vec_end[iter] = this->plot->graph(iter)->dataCount();
        }
        while(1){
            for (uint8_t iter = 0; iter < vec_iter.size(); iter++)
                if (vec_iter[iter] == vec_end[iter]){
                    file.close();
                    return;
                }
            for (uint8_t iter = 0; iter < vec_iter.size(); iter++){
                file_stream << this->plot->graph(iter)->data()->at(vec_iter[iter])->key << ";" << this->plot->graph(iter)->data()->at(vec_iter[iter])->value << ";";
                vec_iter[iter]++;
            }
            file_stream << "\n";
        }
    }
}

void Plot2D::slFullscreen(bool state){
    if (state){
        QPoint position = this->plot->mapToGlobal(this->plot->pos());
        QSize size = this->plot->size();

        this->fullscreen = new QMainWindow;
        this->fullscreen->setWindowTitle("2D Plot");
        this->fullscreen->setAttribute(Qt::WA_DeleteOnClose);
        this->fullscreen->setWindowFlags(this->fullscreen->windowFlags() | Qt::WindowStaysOnTopHint);
        this->fullscreen->setCentralWidget(this->plot);
        this->fullscreen->setGeometry(position.x(), position.y(), size.width(), size.height());
        connect(this->fullscreen, &QMainWindow::destroyed, this, &Plot2D::slFullscreenQuit);
        this->fullscreen->show();
    }
    else{
        delete this->fullscreen;
    }
}

void Plot2D::slFullscreenQuit(void){
    this->plot_parent->layout()->addWidget(this->plot);
    QAction * p_act = this->plot_menu.findChild<QAction *>("fullscreen");
    p_act->setChecked(false);
}

void Plot2D::slCalibrateZero(void){
    emit this->siCalibrateZero();
}

void Plot2D::slStopDrive(void){
    emit this->siStopDrive();
}

void Plot2D::slRescaleToDefault(void){
    this->plot->rescaleAxes();
    this->plot->replot();
}

void Plot2D::slBlockModule(void){
    this->lineedit->setEnabled(false);
    this->dial->setEnabled(false);
    this->pb_calibzero->setEnabled(false);
    this->pb_stop->setEnabled(false);
    this->pb_jogless_b->setEnabled(false);
    this->pb_jogless_s->setEnabled(false);
    this->pb_jogmore_b->setEnabled(false);
    this->pb_jogmore_s->setEnabled(false);
}

void Plot2D::slEnableModule(void){
    this->lineedit->setEnabled(true);
    this->dial->setEnabled(true);
    this->pb_calibzero->setEnabled(true);
    this->pb_stop->setEnabled(true);
    this->pb_jogless_b->setEnabled(true);
    this->pb_jogless_s->setEnabled(true);
    this->pb_jogmore_b->setEnabled(true);
    this->pb_jogmore_s->setEnabled(true);
}

void Plot2D::slJogLessSmall(void){
    this->siSendPos(this->setpoint_position_rads - 0.1);
}
void Plot2D::slJogLessMuch(void){
    this->siSendPos(this->setpoint_position_rads - 1.0);
}
void Plot2D::slJogMoreSmall(void){
    this->siSendPos(this->setpoint_position_rads + 0.1);
}
void Plot2D::slJogMoreMuch(void){
    this->siSendPos(this->setpoint_position_rads + 1.0);
}

void Plot2D::slReceiveActualPosition(float fb_rads, float sp_rads){
    this->actual_position_rads = fb_rads;
    this->setpoint_position_rads = sp_rads;
    if (!((this->dial->hasFocus()) || (this->lineedit->hasFocus()))){
        this->dial_counter = floor(fb_rads / 6.28);
        this->dial->setValue((int)(fb_rads / 6.28 * (this->dial->maximum() + 1)) % (this->dial->maximum() + 1));
        this->dial_old_value = this->dial->value();
        if (this->rb_length->isChecked()){
            this->lineedit->setText(QString::asprintf("%.4f", this->GetLengthFromAngle(fb_rads)));
        }
        else if (this->rb_rads->isChecked()){
            this->lineedit->setText(QString::asprintf("%.4f", fb_rads));
        }
        else if (this->rb_turn->isChecked()){
            this->lineedit->setText(QString::asprintf("%.4f", fb_rads / 6.28));
        }
    }
}

void Plot2D::slSetPos(void){
    QWidget * set_pos_window = new QWidget();
    set_pos_window->setWindowModality(Qt::ApplicationModal);
    set_pos_window->setWindowTitle("Calibration");
    QBoxLayout * layout = new QBoxLayout(QBoxLayout::TopToBottom);
    QPushButton * set_button = new QPushButton("Set Position");
    this->lineedit_setpos = new QLineEdit();
    QDoubleValidator * value_validator = new QDoubleValidator(0, 5, 3);
    this->lineedit_setpos->setValidator(value_validator);
    QLabel * label = new QLabel("Rope Length, m");

    layout->addWidget(label);
    layout->addWidget(this->lineedit_setpos);
    layout->addWidget(set_button);

    set_pos_window->setLayout(layout);
    set_pos_window->move(QCursor::pos());
    connect(set_pos_window, &QWidget::destroyed, set_pos_window, &QWidget::deleteLater);
    connect(set_button, &QPushButton::released, this, &Plot2D::slSendSetPos);
    connect(this, &Plot2D::siSendSetPos, set_pos_window, &QWidget::deleteLater);
    set_pos_window->show();
}

void Plot2D::slSendSetPos(void){
    QString str_val = this->lineedit_setpos->text();

    for (auto iter = str_val.begin(); iter != str_val.end(); iter++){
        if ((*iter) == ',') (*iter) = '.';
    }
    float fl_value = str_val.toFloat();

    emit this->siSendSetPos(fl_value);
}
