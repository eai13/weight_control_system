#include "app2_upperthresblock.h"

APP2_upperthresblock::APP2_upperthresblock(QGraphicsScene * parent, float thres){
    this->output = new APP2_signalnode("Output");
    this->signalnodes.push_back(this->output);

    this->input = new APP2_slotnode("Input");
    this->slotnodes.push_back(this->input);

    this->mommy_canvas = parent;
    this->mommy_canvas->addItem(this->output);
    this->mommy_canvas->addItem(this->input);

    this->thres_label = new APP2_valuelabel("MAX", thres);

    QList<APP2_valuelabel *>vllist;
    vllist.push_back(this->thres_label);
    this->mommy_canvas->addItem(this->thres_label);

    this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(this, QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Upper Thres"), vllist));
    this->mommy_canvas->addItem(this->resizenode = new APP2_resizenode);

    connect(this->simpleblock, &APP2_simpleblock::siSettingsMenuShow, this, &APP2_upperthresblock::slSettingsMenuShow);
    connect(this->simpleblock, &APP2_simpleblock::siMainBlockDeleted, this->resizenode, &APP2_resizenode::slMainBlockDeleted);
    connect(this->simpleblock, &APP2_simpleblock::siMoved, this->resizenode, &APP2_resizenode::slMoveTo);
    connect(this->resizenode, &APP2_resizenode::siMoved, this->simpleblock, &APP2_simpleblock::slResize);
    this->thres = thres;
}

void APP2_upperthresblock::slSettingsMenuShow(void){
    QWidget * dialog_window = new QWidget;
    dialog_window->setWindowTitle("Upper Thres");
    QGridLayout * grid_layout = new QGridLayout;
    this->tmp_thres_line = new QLineEdit(QString::asprintf("%.2f", this->thres));
    this->tmp_thres_line->setValidator(new QDoubleValidator(-1000, 1000, 3));
    QPushButton * button_ok = new QPushButton("Accept");
    QPushButton * button_cancel = new QPushButton("Cancel");
    grid_layout->addWidget(new QLabel("Upper Thres:"), 0, 0, 1, 2);
    grid_layout->addWidget(this->tmp_thres_line, 1, 0, 1, 2);
    grid_layout->addWidget(button_ok, 2, 0, 1, 1);
    grid_layout->addWidget(button_cancel, 2, 1, 1, 1);
    dialog_window->setLayout(grid_layout);

    connect(button_ok, &QPushButton::clicked, this, &APP2_upperthresblock::slSetValues);
    connect(button_ok, &QPushButton::clicked, dialog_window, &QWidget::deleteLater);
    connect(button_cancel, &QPushButton::clicked, dialog_window, &QWidget::deleteLater);

    dialog_window->show();
}

void APP2_upperthresblock::ProcessBlockData(void){
    this->input->previous_value = this->input->current_value;
    this->input->current_value = this->input->connect_line->sig->previous_value;

    if (this->input->current_value > this->thres){
        this->output->current_value = this->thres;
    }
    else{
        this->output->current_value = this->input->current_value;
    }

    this->simpleblock->is_processed = true;
    qDebug() << "UPPER THRES BLOCK Signal " << this->output->current_value;
}
