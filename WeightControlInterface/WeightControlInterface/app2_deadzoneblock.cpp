#include "app2_deadzoneblock.h"

APP2_deadzoneblock::APP2_deadzoneblock(QGraphicsScene * parent, float upper, float lower){
    this->output = new APP2_signalnode("Output");
    this->signalnodes.push_back(this->output);

    this->input = new APP2_slotnode("Input");
    this->slotnodes.push_back(this->input);

    this->mommy_canvas = parent;
    this->mommy_canvas->addItem(this->output);
    this->mommy_canvas->addItem(this->input);

    this->lower_line_label = new APP2_valuelabel("MIN", lower);
    this->upper_line_label = new APP2_valuelabel("MAX", upper);

    QList<APP2_valuelabel *> vllist;
    vllist.push_back(this->lower_line_label);
    vllist.push_back(this->upper_line_label);
    this->mommy_canvas->addItem(this->lower_line_label);
    this->mommy_canvas->addItem(this->upper_line_label);

    this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(this, QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Deadzone"), vllist));
    this->mommy_canvas->addItem(this->resizenode = new APP2_resizenode);

    connect(this->simpleblock, &APP2_simpleblock::siSettingsMenuShow, this, &APP2_deadzoneblock::slSettingsMenuShow);
    connect(this->simpleblock, &APP2_simpleblock::siMainBlockDeleted, this->resizenode, &APP2_resizenode::slMainBlockDeleted);
    connect(this->simpleblock, &APP2_simpleblock::siMoved, this->resizenode, &APP2_resizenode::slMoveTo);
    connect(this->resizenode, &APP2_resizenode::siMoved, this->simpleblock, &APP2_simpleblock::slResize);
    this->lower_line = lower;
    this->upper_line = upper;
}

void APP2_deadzoneblock::slSettingsMenuShow(void){
    QWidget * dialog_window = new QWidget;
    dialog_window->setWindowTitle("Threshold");
    QGridLayout * grid_layout = new QGridLayout;
    this->tmp_lower_line = new QLineEdit(QString::asprintf("%.2f", this->upper_line));
    this->tmp_lower_line->setValidator(new QDoubleValidator(-1000, 1000, 3));
    this->tmp_upper_line = new QLineEdit(QString::asprintf("%.2f", this->lower_line));
    this->tmp_upper_line->setValidator(new QDoubleValidator(-1000, 1000, 3));
    QPushButton * button_ok = new QPushButton("Accept");
    QPushButton * button_cancel = new QPushButton("Cancel");
    grid_layout->addWidget(new QLabel("Upper Line:"), 0, 0, 1, 2);
    grid_layout->addWidget(this->tmp_upper_line, 1, 0, 1, 2);
    grid_layout->addWidget(new QLabel("Lower Line:"), 2, 0, 1, 2);
    grid_layout->addWidget(this->tmp_lower_line, 3, 0, 1, 2);
    grid_layout->addWidget(button_ok, 4, 0, 1, 1);
    grid_layout->addWidget(button_cancel, 4, 1, 1, 1);
    dialog_window->setLayout(grid_layout);

    connect(button_ok, &QPushButton::clicked, this, &APP2_deadzoneblock::slSetValues);
    connect(button_ok, &QPushButton::clicked, dialog_window, &QWidget::deleteLater);
    connect(button_cancel, &QPushButton::clicked, dialog_window, &QWidget::deleteLater);

    dialog_window->show();
}

void APP2_deadzoneblock::ProcessBlockData(void){
    this->input->previous_value = this->input->current_value;
    this->input->current_value = this->input->connect_line->sig->previous_value;

    if (this->input->current_value > this->upper_line){
        this->output->current_value = this->input->current_value - this->upper_line;
    }
    else if (this->input->current_value < this->lower_line){
        this->output->current_value = this->input->current_value - this->lower_line;
    }
    else{
        this->output->current_value = 0;
    }

    this->simpleblock->is_processed = true;
    qDebug() << "DEADZONE BLOCK Signal " << this->output->current_value;
}

