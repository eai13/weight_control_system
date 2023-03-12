#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QListWidgetItem>
#include <QCompleter>
#include <QStringListModel>
#include "qcustomplot.h"
#include <QTableWidget>

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

    void slGetSimilarInstances(QStringList Instances);

private slots:
    void slClearCommandHistory(void);
    void slClearVariables(void);
    void slInspectVariable(QListWidgetItem * item);
    void slInterpret(void);
    void slTextEdited(QString const & Value);
    void slKeyUpReleased(void);
    void slKeyDownReleased(void);
private:
    uint32_t CommandHistoryItem = 0;
    QString CommandStash;

private:

    Ui::MainWindow *ui;

    QListWidgetItem *  CheckVariableExists(QString Name);

    MathInterpreter Interpreter;

    QStringList Instances;

signals:
    void siVariableRemoved(QString Name);
    void siVariableChanged(QString Name, QString Value);
    void siShowVariableWindow(QString Name);
};
#endif // MAINWINDOW_H
