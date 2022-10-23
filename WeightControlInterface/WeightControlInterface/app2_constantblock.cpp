#include "app2_constantblock.h"

APP2_constantblock::APP2_constantblock(QGraphicsScene * parent, float value){
    this->output = new APP2_signalnode("Value");
    this->signalnodes.push_back(this->output);

    this->mommy_canvas = parent;
    for (auto iter = this->signalnodes.begin(); iter != this->signalnodes.end(); iter++){
        this->mommy_canvas->addItem(*iter);
    }
    for (auto iter = this->slotnodes.begin(); iter != this->slotnodes.end(); iter++){
        this->mommy_canvas->addItem(*iter);
    }
    this->valuelabel = new APP2_valuelabel("", value);
    QList<APP2_valuelabel *> vllist;
    vllist.push_back(this->valuelabel);
    for (auto iter = vllist.begin(); iter != vllist.end(); iter++){
        this->mommy_canvas->addItem(*iter);
    }

    this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(this, QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Constant"), vllist));
    this->mommy_canvas->addItem(this->resizenode = new APP2_resizenode);

    connect(this->simpleblock, &APP2_simpleblock::siSettingsMenuShow, this, &APP2_constantblock::slSettingsMenuShow);
    connect(this->simpleblock, &APP2_simpleblock::siMainBlockDeleted, this->resizenode, &APP2_resizenode::slMainBlockDeleted);
    connect(this->simpleblock, &APP2_simpleblock::siMoved, this->resizenode, &APP2_resizenode::slMoveTo);
    connect(this->resizenode, &APP2_resizenode::siMoved, this->simpleblock, &APP2_simpleblock::slResize);
    this->value = value;
}

void APP2_constantblock::slSettingsMenuShow(void){
    QWidget * dialog_window = new QWidget;
    dialog_window->setWindowTitle("Constant");
    QGridLayout * grid_layout = new QGridLayout;
    grid_layout->addWidget(new QLabel("Value:"), 0, 0, 1, 2);
    this->tmp_value_line = new QLineEdit(QString::asprintf("%.2f", this->value));
    this->tmp_value_line->setValidator(new QDoubleValidator(-1000, 1000, 3));
    QPushButton * button_ok = new QPushButton("Accept");
    QPushButton * button_cancel = new QPushButton("Cancel");
    grid_layout->addWidget(tmp_value_line, 1, 0, 1, 2);
    grid_layout->addWidget(button_ok, 2, 0);
    grid_layout->addWidget(button_cancel, 2, 1);
    dialog_window->setLayout(grid_layout);

    connect(button_ok, &QPushButton::clicked, this, &APP2_constantblock::slSetValue);
    connect(button_ok, &QPushButton::clicked, dialog_window, &QWidget::deleteLater);
    connect(button_cancel, &QPushButton::clicked, dialog_window, &QWidget::deleteLater);

    dialog_window->show();
}

void APP2_constantblock::ProcessBlockData(void){
    this->output->current_value = this->value;
    qDebug() << "CONSTANT SIGNAL VALUE " << this->output->current_value;
}
