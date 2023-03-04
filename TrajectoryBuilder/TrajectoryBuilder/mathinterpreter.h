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

class MathInterpreter
{
public:

    MathInterpreter();
    void InterpretString(QString commString);


private:
    uint32_t CheckBraces(QString const & commString);

    QMap<QString, MathFunctions::AbstractFunction *> Functions;
    QMap<QString, MathOperators::AbstractOperator *> Operators;
    QMap<QString, MathTypes::AbstractType *> Variables;

};

#endif // MATHINTERPRETER_H
