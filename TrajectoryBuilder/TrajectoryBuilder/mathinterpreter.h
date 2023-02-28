#ifndef MATHINTERPRETER_H
#define MATHINTERPRETER_H

#include <QMap>
#include <QString>
#include <QVector>
#include "mathfunctions.h"
#include "mathtypes.h"
#include <QDebug>

class MathInterpreter
{
public:

    MathInterpreter();
    void InterpretString(QString commString);


private:
    QMap<QString, MathFunctions::AbstractFunction *> Functions;
    QMap<QString, MathOperators::AbstractOperator *> Operators;
    QMap<QString, MathTypes::AbstractType *> Variables;

};

#endif // MATHINTERPRETER_H
