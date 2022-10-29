#include "app2_plotblock.h"

APP2_plotblock::APP2_plotblock(QGraphicsScene * parent){
    this->pencolor_buffer.push_back(QColor(255, 0, 0));
    this->pencolor_buffer.push_back(QColor(0, 255, 0));
    this->pencolor_buffer.push_back(QColor(0, 0, 255));
    this->pencolor_buffer.push_back(QColor(255, 0, 255));
    this->pencolor_buffer.push_back(QColor(255, 255, 0));

    this->slotnodes.push_back(new APP2_slotnode("Data"));

    this->plot = new QCustomPlot();
    this->plot->clearItems();
    this->plot->plotLayout()->insertRow(0);
    this->plot->plotLayout()->addElement(0, 0, new QCPTextElement(this->plot, "Plot", QFont("sans", 12, QFont::Bold)));
    this->plot->xAxis->setLabel("Time, s");
    this->plot->yAxis->setLabel("Value");
    this->plot->xAxis->setRange(0, 10);
    this->plot->yAxis->setRange(-1, 1);
    this->plot->legend->setVisible(true);
    this->plot->setInteraction(QCP::iSelectPlottables, true);
    this->plot->addGraph();
    this->plot->graph(0)->setPen(this->pencolor_buffer.takeLast());

    this->mommy_canvas = parent;
    this->mommy_canvas->addItem(this->slotnodes.back());

    QList<APP2_valuelabel *> vllist;

    this->mommy_canvas->addItem(this->simpleblock = new APP2_simpleblock(this, QRectF(0, 0, 100, 100), this->signalnodes, this->slotnodes, this->mommy_canvas->addText("Plot"), vllist));
    this->mommy_canvas->addItem(resizenode = new APP2_resizenode);

    connect(this->simpleblock, &APP2_simpleblock::siSettingsMenuShow, this, &APP2_plotblock::slSettingsMenuShow);
    connect(this->simpleblock, &APP2_simpleblock::siMainBlockDeleted, this->resizenode, &APP2_resizenode::slMainBlockDeleted);
    connect(this->simpleblock, &APP2_simpleblock::siMoved, this->resizenode, &APP2_resizenode::slMoveTo);
    connect(this->resizenode, &APP2_resizenode::siMoved, this->simpleblock, &APP2_simpleblock::slResize);
}

void APP2_plotblock::slSettingsMenuShow(void){
    QWidget * dialog_window = new QWidget;
    dialog_window->setWindowTitle("Plot");
    dialog_window->setWindowFlags(dialog_window->windowFlags() | Qt::WindowStaysOnTopHint);
    QGridLayout * grid_layout = new QGridLayout;
    grid_layout->addWidget(this->plot, 0, 0, 1, 4);

    QComboBox * data_amount = new QComboBox;
    for (uint8_t iter = 1; iter < 6; iter++)
        data_amount->addItem(QString::number(iter));
    data_amount->setCurrentIndex(this->plot->graphCount() - 1);
    grid_layout->addWidget(new QLabel("Plottables Amount:"), 1, 0, 1, 2);
    grid_layout->addWidget(data_amount, 1, 2, 1, 2);

    connect(data_amount, &QComboBox::currentTextChanged, this, &APP2_plotblock::slSetSlotsAmount);

    dialog_window->setLayout(grid_layout);
    dialog_window->setMinimumSize(400, 400);

    dialog_window->show();
}

void APP2_plotblock::ProcessBlockData(void){
    uint8_t iter = 0;
    for (auto slt = this->slotnodes.begin(); slt != this->slotnodes.end(); slt++){
        if ((*slt)->connect_line != nullptr){
            (*slt)->previous_value = (*slt)->current_value;
            (*slt)->current_value = (*slt)->connect_line->sig->previous_value;
        }
        this->plot->graph(iter)->addData(this->plot->graph(iter)->dataCount(), (*slt)->current_value);
        iter++;
    }
    this->plot->rescaleAxes();
    this->plot->replot();
//    this->input->previous_value = this->input->current_value;
//    this->input->current_value = this->input->connect_line->sig->previous_value;

//    this->plot->graph(0)->addData(this->plot->graph(0)->dataCount(), this->input->current_value);
//    this->plot->rescaleAxes();
//    this->plot->replot();
}
