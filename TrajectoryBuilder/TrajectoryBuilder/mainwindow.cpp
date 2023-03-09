#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mathinterpreter.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pb_ClearCommandHistory, &QPushButton::released, this, &MainWindow::slClearCommandHistory);
    connect(ui->pb_ClearVariables,      &QPushButton::released, this, &MainWindow::slClearVariables);
    connect(ui->pb_Interpret,           &QPushButton::released, this, &MainWindow::slInterpret);
    connect(ui->line_Interpreter,       &QLineEdit::returnPressed, this, &MainWindow::slInterpret);
    connect(ui->line_Interpreter,       &QLineEdit::textEdited, this, &MainWindow::slTextEdited);

    connect(&(this->Interpreter), &MathInterpreter::siInterpreterDebugString, this, &MainWindow::slLogDebug);
    connect(&(this->Interpreter), &MathInterpreter::siVariableCreated, this, &MainWindow::slVariableCreated);
    connect(&(this->Interpreter), &MathInterpreter::siVariableRemoved, this, &MainWindow::slVariableRemoved);
    connect(this, &MainWindow::siVariableChanged, &(this->Interpreter), &MathInterpreter::slVariableChanged);
    connect(this, &MainWindow::siVariableRemoved, &(this->Interpreter), &MathInterpreter::slVariableRemoved);
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::slLogWarning(QString log){
    if (!(ui->cb_Verbose->isChecked())) return;
    ui->list_Log->addItem(log);
    ui->list_Log->item(ui->list_Log->count() - 1)->setTextColor(Qt::yellow);
    ui->list_Log->scrollToBottom();
}

void MainWindow::slLogError(QString log){
    ui->list_Log->addItem(log);
    ui->list_Log->item(ui->list_Log->count() - 1)->setTextColor(Qt::red);
    ui->list_Log->scrollToBottom();
}

void MainWindow::slLogDebug(QString log){
    ui->list_Log->addItem(log);
    ui->list_Log->scrollToBottom();
}

void MainWindow::slVariableCreated(QString Name){
    ui->list_Variables->addItem(Name);
}

void MainWindow::slVariableRemoved(QString Name){
    QListWidgetItem * ItemPlace = this->CheckVariableExists(Name);
    if (ItemPlace != nullptr){
        delete ItemPlace;
    }
}

void MainWindow::slAddCommandToHistory(QString Command){
    ui->list_CommandHistory->addItem(Command);
}

void MainWindow::slGetSimilarInstances(QStringList Instances){
    QPoint HelpMenuPos = QWidget::mapToGlobal(ui->line_Interpreter->pos()) + QPoint(0, ui->line_Interpreter->height());


}

void MainWindow::slClearCommandHistory(void){
    ui->list_CommandHistory->clear();
}

void MainWindow::slClearVariables(void){
    for (int iter = ui->list_Variables->count() - 1; iter >= 0; iter--){
        qDebug() << ui->list_Variables->item(iter)->text();
        emit this->siVariableRemoved(ui->list_Variables->item(iter)->text());
        ui->list_Variables->removeItemWidget(ui->list_Variables->item(iter));
    }
}

void MainWindow::slInterpret(void){
    this->slAddCommandToHistory(ui->line_Interpreter->text());
    this->slLogDebug("STARTED");
    this->Interpreter.InterpretString(ui->line_Interpreter->text());
    this->slLogDebug("ENDED");
    ui->line_Interpreter->clear();
}

void MainWindow::slTextEdited(QString const & Value){
    QString CurrentInstance;
}

QListWidgetItem * MainWindow::CheckVariableExists(QString Name){
    QList<QListWidgetItem *> Items = ui->list_Variables->findItems(Name, Qt::MatchExactly);
    if (Items.size()){
        return Items[0];
    }
    else{
        return nullptr;
    }
}
