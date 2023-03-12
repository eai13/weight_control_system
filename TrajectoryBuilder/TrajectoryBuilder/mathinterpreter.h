#ifndef MATHINTERPRETER_H
#define MATHINTERPRETER_H

#include <QMap>
#include <QString>
#include <QVector>
#include <QStack>
#include "mathfunctions.h"
#include "mathtypes.h"
#include "mathtreeprocessor.h"
#include <QDebug>
#include <QWidget>
#include <QObject>
#include <QBoxLayout>

//QT_BEGIN_NAMESPACE
//namespace Interpreter { class MathInterpreter; }
//QT_END_NAMESPACE

class MathInterpreter : public QObject
{
    Q_OBJECT

public:

    explicit MathInterpreter(QObject * parent = nullptr);
    ~MathInterpreter();

    void InterpretString(QString commString);

private:
    bool CheckBraces(QString const & commString);
    QStringList InterpretSubstring(QStringList Input);
    QStringList StringPreprocessor(QString input);

    QMap<QString, MathFunctions::AbstractFunction *> Functions;
    QMap<QString, MathOperators::AbstractOperator *> Operators;
    QMap<QString, MathTypes::AbstractType *> Variables;

public slots:
    void slShowVariableWindow(QString Name);
    void slVariableRemoved(QString Name);
    void slVariableChanged(QString Name, QString Value);

signals:
    void siVariableCreated(QString Name);
    void siVariableRemoved(QString Name);
    void siInterpreterDebugString(QString Name);

};

#endif // MATHINTERPRETER_H
