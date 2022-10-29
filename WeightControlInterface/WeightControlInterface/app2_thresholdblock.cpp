#include "app2_thresholdblock.h"

APP2_thresholdblock::APP2_thresholdblock(QGraphicsScene * parent, float max, float min){
    this->output = new APP2_signalnode("Output");
    this->signalnodes.push_back(this->output);

    this->input = new APP2_slotnode("Input");
    this->slotnodes.push_back(this->input);

    this->mommy_canvas = parent;
    this->mommy_canvas->addItem(this->output);
    this->mommy_canvas->addItem(this->input);

    this->max_label = new APP2_valuelabel("MAX", max);
    this->min_label = new APP2_valuelabel("MIN", min);

    QList<APP2_valuelabel *> vllist;
    vllist.push_back(this->max_label);
    vllist.push_back(this->min_label);
    this->mommy_canvas->addItem(this->max_label);
    this->mommy_canvas->addItem(this->min_label);

    this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(this, QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Threshold"), vllist));
    this->mommy_canvas->addItem(this->resizenode = new APP2_resizenode);

    connect(this->simpleblock, &APP2_simpleblock::siSettingsMenuShow, this, &APP2_thresholdblock::slSettingsMenuShow);
    connect(this->simpleblock, &APP2_simpleblock::siMainBlockDeleted, this->resizenode, &APP2_resizenode::slMainBlockDeleted);
    connect(this->simpleblock, &APP2_simpleblock::siMoved, this->resizenode, &APP2_resizenode::slMoveTo);
    connect(this->resizenode, &APP2_resizenode::siMoved, this->simpleblock, &APP2_simpleblock::slResize);
    this->max = max;
    this->min = min;
}

void APP2_thresholdblock::slSettingsMenuShow(void){
    QWidget * dialog_window = new QWidget;
    dialog_window->setWindowTitle("Threshold");
    QGridLayout * grid_layout = new QGridLayout;
    this->tmp_max_value_line = new QLineEdit(QString::asprintf("%.2f", this->max));
    this->tmp_max_value_line->setValidator(new QDoubleValidator(-1000, 1000, 3));
    this->tmp_min_value_line = new QLineEdit(QString::asprintf("%.2f", this->min));
    this->tmp_min_value_line->setValidator(new QDoubleValidator(-1000, 1000, 3));
    QPushButton * button_ok = new QPushButton("Accept");
    QPushButton * button_cancel = new QPushButton("Cancel");
    grid_layout->addWidget(new QLabel("Upper Thres:"), 0, 0, 1, 2);
    grid_layout->addWidget(this->tmp_max_value_line, 1, 0, 1, 2);
    grid_layout->addWidget(new QLabel("Lower Thres:"), 2, 0, 1, 2);
    grid_layout->addWidget(this->tmp_min_value_line, 3, 0, 1, 2);
    grid_layout->addWidget(button_ok, 4, 0, 1, 1);
    grid_layout->addWidget(button_cancel, 4, 1, 1, 1);
    dialog_window->setLayout(grid_layout);

    connect(button_ok, &QPushButton::clicked, this, &APP2_thresholdblock::slSetValues);
    connect(button_ok, &QPushButton::clicked, dialog_window, &QWidget::deleteLater);
    connect(button_cancel, &QPushButton::clicked, dialog_window, &QWidget::deleteLater);

    dialog_window->show();
}

void APP2_thresholdblock::ProcessBlockData(void){
    this->input->previous_value = this->input->current_value;
    this->input->current_value = this->input->connect_line->sig->previous_value;

    if (this->input->current_value > this->max){
        this->output->current_value = this->max;
    }
    else if (this->input->current_value < this->min){
        this->output->current_value = this->min;
    }
    else{
        this->output->current_value = this->input->current_value;
    }

    this->simpleblock->is_processed = true;
    qDebug() << "THRESHOLD BLOCK Signal " << this->output->current_value;
}
