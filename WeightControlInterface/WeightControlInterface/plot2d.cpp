#include "plot2d.h"

Plot2D::Plot2D(QString title, QCustomPlot * plot_h,
               QRadioButton * rb_turn_h, QRadioButton * rb_rads_h, QRadioButton * rb_length_h,
               QDial * dial_h,
               QLineEdit * lineedit_h,
               QPushButton * zerocalib, QPushButton * movezero,
               double R0, double K, double MIN, double MAX){
    this->plot_parent = plot_h->parentWidget();
    this->plot = plot_h;
    this->rb_turn = rb_turn_h;
    this->rb_rads = rb_rads_h;
    this->rb_length = rb_length_h;
    this->lineedit = lineedit_h;
    this->dial = dial_h;
    this->pb_calibzero = zerocalib;
    this->pb_movezero = movezero;

    this->R0_calib = R0;
    this->K_calib = K;
    this->MIN_calib = MIN;
    this->MAX_calib = MAX;

    this->plot->clearItems();
    this->plot->plotLayout()->insertRow(0);
    this->plot->plotLayout()->addElement(0, 0, new QCPTextElement(this->plot, title, QFont("sans", 12, QFont::Bold)));
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
    connect(p_act, &QAction::triggered, this, &Plot2D::slPlotActive);

    this->plot_menu.addSeparator();

    p_act = this->plot_menu.addAction("Auto Rescale");
    p_act->setObjectName("Autorescale");
    p_act->setCheckable(true);

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

    connect(this->lineedit, &QLineEdit::returnPressed, this, &Plot2D::slProcessEditLine);
    connect(this->rb_turn, &QRadioButton::clicked, this, &Plot2D::slSetTurns);
    connect(this->rb_rads, &QRadioButton::clicked, this, &Plot2D::slSetRads);
    connect(this->rb_length, &QRadioButton::clicked, this, &Plot2D::slSetLength);
    connect(this->dial, &QDial::sliderMoved, this, &Plot2D::slProcessDial);
    connect(this->dial, &QDial::sliderReleased, this, &Plot2D::slSendPosFromDial);
    connect(this->pb_calibzero, &QPushButton::released, this, &Plot2D::slCalibrateZero);
    connect(this->pb_movezero, &QPushButton::released, this, &Plot2D::slSetZero);

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

    for (uint8_t iter = 0; iter < this->plot->graphCount(); iter++)
        this->plot->graph(iter)->data()->clear();

    this->plot->replot();
    this->system_time->restart();
}

void Plot2D::ResetPlot(void){
    this->lineedit->setText("0,000");
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
        this->lineedit->setText(QString::fromStdString(std::to_string(
            float(data + this->dial_counter * (this->dial->maximum() + 1)) / (this->dial->maximum() + 1))));
    }
    else if (this->rb_rads->isChecked()){
        this->lineedit->setText(QString::fromStdString(std::to_string(
            float(data + this->dial_counter * (this->dial->maximum() + 1)) / (this->dial->maximum() + 1) * 6.28)));
    }
    else if (this->rb_length->isChecked()){
        float angle = float(data + this->dial_counter * (this->dial->maximum() + 1)) / (this->dial->maximum() + 1) * 6.28;
        float length = this->GetLengthFromAngle(angle); //(this->R0_calib + (this->MAX_calib - angle) * this->K_calib / 2 / PI) * (angle - this->MIN_calib);
        this->lineedit->setText(QString::asprintf("%.2f", length));
    }
}

void Plot2D::slSendPosFromDial(void){
    QString str = this->lineedit->text();
    for (auto iter = str.begin(); iter != str.end(); iter++)
        if (*iter == ',') *iter = '.';
    if (this->rb_rads->isChecked()){
        if (str.toFloat() > this->MAX_calib){
            this->lineedit->setText(QString::asprintf("%.2f", this->MAX_calib));
            emit siSendPos(this->MAX_calib);
        }
        else if (str.toFloat() < this->MIN_calib){
            this->lineedit->setText(QString::asprintf("%.2f", this->MIN_calib));
            emit siSendPos(this->MIN_calib);
        }
        else{
            emit siSendPos(str.toFloat());
        }
    }
    else if (this->rb_turn->isChecked()){
        if ((str.toFloat() * 6.28) > this->MAX_calib){
            this->lineedit->setText(QString::asprintf("%.2f", this->MAX_calib / 6.28));
            emit siSendPos(this->MAX_calib / 6.28);
        }
        else if ((str.toFloat() * 6.28) < this->MIN_calib){
            this->lineedit->setText(QString::asprintf("%.2f", this->MIN_calib / 6.28));
            emit siSendPos(this->MIN_calib / 6.28);
        }
        else{
            emit siSendPos(str.toFloat() * 6.28);
        }
    }
    else if (this->rb_length->isChecked()){
        float angle = this->GetAngleFromLength(str.toFloat());
        if (angle > this->MAX_calib){
            float new_length = this->GetLengthFromAngle(this->MAX_calib);// = (this->R0_calib + (this->MAX_calib - this->MAX_calib) * this->K_calib / 2 / PI) * (this->MAX_calib - this->MIN_calib);
            this->lineedit->setText(QString::asprintf("%.2f", new_length));
            emit siSendPos(this->MAX_calib);
        }
        else if (angle < this->MIN_calib){
            float new_length = this->GetLengthFromAngle(this->MIN_calib);//this->R0_calib + (this->MAX_calib - this->MIN_calib) * this->K_calib / 2 / PI) * (this->MIN_calib - this->MIN_calib);
            this->lineedit->setText(QString::asprintf("%.2f", new_length));
            emit siSendPos(this->MIN_calib);
        }
        else{
            emit siSendPos(angle);
        }
        qDebug() << "ANGLE FROM LENGTH IS: " << angle;
    }
}

void Plot2D::slSetTurns(void){
    float new_val = (float)(this->dial_counter * (this->dial->maximum() + 1) + this->dial->value()) / (float)(this->dial->maximum());
    this->lineedit->setText(QString::asprintf("%.2f", new_val));
}

void Plot2D::slSetRads(void){
    float new_val = (float)(this->dial_counter * (this->dial->maximum() + 1) + this->dial->value()) / (float)(this->dial->maximum()) * 6.28;
    this->lineedit->setText(QString::asprintf("%.2f", new_val));
}

void Plot2D::slSetLength(void){
    float angle = (float)(this->dial_counter * (this->dial->maximum() + 1) + this->dial->value()) / (float)(this->dial->maximum()) * 6.28;
    float new_length = this->GetLengthFromAngle(angle); //this->R0_calib + (this->MAX_calib - angle) * this->K_calib / 2 / PI) * (angle - this->MIN_calib);
    this->lineedit->setText(QString::asprintf("%.2f", new_length));
}

void Plot2D::slShowContextMenu(const QPoint & pos){
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

void Plot2D::slSetZero(void){
    if (this->rb_length->isChecked()){
        float new_length = (this->R0_calib + (this->MAX_calib - 0) * this->K_calib / 2 / PI) * (0 - this->MIN_calib);
        this->lineedit->setText(QString::asprintf("%.2f", new_length));
    }
    else if (this->rb_rads->isChecked()){
        this->lineedit->setText("0.00");
    }
    else if (this->rb_turn->isChecked()){
        this->lineedit->setText("0.00");
    }
    this->slProcessEditLine();
}

void Plot2D::PushLength(float length){
    if (this->rb_length->isChecked()){
        this->lineedit->setText(QString::asprintf("%.2f", length));
    }
    else if (this->rb_rads->isChecked()){
        float angle = this->GetAngleFromLength(length);
        this->lineedit->setText(QString::asprintf("%.2f", angle));
    }
    else if (this->rb_turn->isChecked()){
        float angle = this->GetAngleFromLength(length);
        this->lineedit->setText(QString::asprintf("%.2f", angle / 2 / PI));
    }
    this->slProcessEditLine();
}
void Plot2D::PushRadians(float rads){
    if (this->rb_length->isChecked()){
        float new_length = this->GetLengthFromAngle(rads);// (this->R0_calib + (this->MAX_calib - rads) * this->K_calib / 2 / PI) * (rads - this->MIN_calib);
    }
    else if (this->rb_rads->isChecked()){
        this->lineedit->setText(QString::asprintf("%.2f", rads));
    }
    else if (this->rb_turn->isChecked()){
        this->lineedit->setText(QString::asprintf("%.2f", rads / 2 / PI));
    }
    this->slProcessEditLine();
}
void Plot2D::PushTurns(float turns){
    if (this->rb_length->isChecked()){
        float new_length = this->GetLengthFromAngle(turns * 2 * PI); //this->R0_calib + (this->MAX_calib - (turns * 2 * PI)) * this->K_calib / 2 / PI) * ((turns * 2 * PI) - this->MIN_calib);
    }
    else if (this->rb_rads->isChecked()){
        this->lineedit->setText(QString::asprintf("%.2f", turns * 2 * PI));
    }
    else if (this->rb_turn->isChecked()){
        this->lineedit->setText(QString::asprintf("%.2f", turns));
    }
    this->slProcessEditLine();
}
