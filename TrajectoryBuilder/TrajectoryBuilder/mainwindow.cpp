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
    connect(ui->line_Interpreter,       &SmartLine::returnPressed, this, &MainWindow::slInterpret);
    connect(ui->line_Interpreter,       &SmartLine::textEdited, this, &MainWindow::slTextEdited);
    connect(ui->line_Interpreter,       &SmartLine::siKeyUpReleased, this, &MainWindow::slKeyUpReleased);
    connect(ui->line_Interpreter,       &SmartLine::siKeyDownReleased, this, &MainWindow::slKeyDownReleased);
    connect(ui->list_Variables,         &QListWidget::itemDoubleClicked, this, &MainWindow::slInspectVariable);

    connect(&(this->Interpreter), &MathInterpreter::siInterpreterDebugString, this, &MainWindow::slLogDebug);
    connect(&(this->Interpreter), &MathInterpreter::siVariableCreated, this, &MainWindow::slVariableCreated);
    connect(&(this->Interpreter), &MathInterpreter::siVariableRemoved, this, &MainWindow::slVariableRemoved);
    connect(this, &MainWindow::siShowVariableWindow, &(this->Interpreter), &MathInterpreter::slShowVariableWindow);
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

void MainWindow::slInspectVariable(QListWidgetItem *item){
    emit this->siShowVariableWindow(item->text());
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

void MainWindow::slKeyUpReleased(void){
    if (ui->list_CommandHistory->count() == 0) return;
    if (this->CommandHistoryItem == 0){
        this->CommandHistoryItem = ui->list_CommandHistory->count() - 1;
    }
    else{
        this->CommandHistoryItem--;
    }
    if (ui->list_CommandHistory->findItems(ui->line_Interpreter->text(), Qt::MatchExactly).isEmpty()){
        this->CommandStash = ui->line_Interpreter->text();
    }
    ui->line_Interpreter->setText(ui->list_CommandHistory->item(this->CommandHistoryItem)->text());
}

void MainWindow::slKeyDownReleased(void){
    ui->line_Interpreter->setText(this->CommandStash);
    this->CommandHistoryItem = ui->list_CommandHistory->count();
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
