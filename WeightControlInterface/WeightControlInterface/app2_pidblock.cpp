#include "app2_pidblock.h"

APP2_PIDblock::APP2_PIDblock(QGraphicsScene * parent){
    this->mommy_canvas = parent;

    this->output = new APP2_signalnode("Error");
    this->signalnodes.push_back(this->output);
    this->mommy_canvas->addItem(this->output);

    this->input = new APP2_slotnode("Control");
    this->slotnodes.push_back(this->input);
    this->mommy_canvas->addItem(this->input);

    QList<APP2_valuelabel *> vllist;
    this->P_label = new APP2_valuelabel("P", this->P);
    this->I_label = new APP2_valuelabel("I", this->I);
    this->D_label = new APP2_valuelabel("D", this->D);
    vllist.push_back(this->P_label);
    vllist.push_back(this->I_label);
    vllist.push_back(this->D_label);
    for (auto iter = vllist.begin(); iter != vllist.end(); iter++)
        this->mommy_canvas->addItem(*iter);

    this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(this, QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("PID"), vllist));
    this->mommy_canvas->addItem(this->resizenode = new APP2_resizenode);

    connect(this->simpleblock, &APP2_simpleblock::siSettingsMenuShow, this, &APP2_PIDblock::slSettingsMenuShow);
    connect(this->simpleblock, &APP2_simpleblock::siMainBlockDeleted, this->resizenode, &APP2_resizenode::slMainBlockDeleted);
    connect(this->simpleblock, &APP2_simpleblock::siMoved, this->resizenode, &APP2_resizenode::slMoveTo);
    connect(this->resizenode, &APP2_resizenode::siMoved, this->simpleblock, &APP2_simpleblock::slResize);
}

void APP2_PIDblock::slSettingsMenuShow(void){
    QWidget * dialog_window = new QWidget;
    dialog_window->setWindowTitle("PID");
    QGridLayout * grid_layout = new QGridLayout;
    grid_layout->addWidget(new QLabel("P:"), 0, 0, 1, 1);
    grid_layout->addWidget(new QLabel("I:"), 1, 0, 1, 1);
    grid_layout->addWidget(new QLabel("D:"), 2, 0, 1, 1);
    this->tmp_P_line = new QLineEdit(QString::asprintf("%.2f", this->P));
    this->tmp_P_line->setValidator(new QDoubleValidator(0, 1000, 3));
    this->tmp_I_line = new QLineEdit(QString::asprintf("%.2f", this->I));
    this->tmp_I_line->setValidator(new QDoubleValidator(0, 1000, 3));
    this->tmp_D_line = new QLineEdit(QString::asprintf("%.2f", this->D));
    this->tmp_D_line->setValidator(new QDoubleValidator(0, 1000, 3));
    grid_layout->addWidget(this->tmp_P_line, 0, 1, 1, 1);
    grid_layout->addWidget(this->tmp_I_line, 1, 1, 1, 1);
    grid_layout->addWidget(this->tmp_D_line, 2, 1, 1, 1);

    this->tmp_I_antiwindup_check = new QCheckBox("AntiWindup Enabled");
    if (this->I_antiwindup_enabled)
        this->tmp_I_antiwindup_check->setChecked(true);
    else
        this->tmp_I_antiwindup_check->setChecked(true);
    grid_layout->addWidget(this->tmp_I_antiwindup_check, 3, 0, 1, 2);

    this->tmp_I_antiwindup_line = new QLineEdit(QString::asprintf("%.2f", this->I_antiwindup));
    this->tmp_I_antiwindup_line->setValidator(new QDoubleValidator(0, 1000, 3));
    grid_layout->addWidget(new QLabel("AntiWindup:"), 4, 0, 1, 1);
    grid_layout->addWidget(this->tmp_I_antiwindup_line, 4, 1, 1, 1);

    QPushButton * button_ok = new QPushButton("Accept");
    QPushButton * button_cancel = new QPushButton("Cancel");
    grid_layout->addWidget(button_ok, 5, 0);
    grid_layout->addWidget(button_cancel, 5, 1);

    connect(button_ok, &QPushButton::clicked, this, &APP2_PIDblock::slSetValue);
    connect(button_ok, &QPushButton::clicked, dialog_window, &QWidget::deleteLater);
    connect(button_cancel, &QPushButton::clicked, dialog_window, &QWidget::deleteLater);

    dialog_window->setLayout(grid_layout);
    dialog_window->show();
}

void APP2_PIDblock::ProcessBlockData(void){
    qDebug() << "PID BLOCK VALUE " << this->output->current_value;
}
