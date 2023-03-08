#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QListWidgetItem>

#include "mathinterpreter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void slLogWarning(QString log);
    void slLogError(QString log);
    void slLogDebug(QString log);

    void slVariableCreated(QString Name);
    void slVariableRemoved(QString Name);

    void slAddCommandToHistory(QString Command);

private slots:
    void slClearCommandHistory(void);
    void slClearVariables(void);
    void slInterpret(void);

private:
    Ui::MainWindow *ui;

    QListWidgetItem *  CheckVariableExists(QString Name);

    MathInterpreter Interpreter;

signals:

    void siVariableRemoved(QString Name);
    void siVariableChanged(QString Name, QString Value);
};
#endif // MAINWINDOW_H
