#include "mathfunctions.h"
#include <QVector>


MathTypes::AbstractType &MathOperators::MathOperatorSum::Execute(MathTypes::AbstractType &arg_l, MathTypes::AbstractType &arg_r){
    return arg_l.sum(arg_r);
}

MathTypes::AbstractType &MathOperators::MathOperatorSubtract::Execute(MathTypes::AbstractType &arg_l, MathTypes::AbstractType &arg_r){
    return arg_l.subtract(arg_r);
}

MathTypes::AbstractType &MathOperators::MathOperatorDivision::Execute(MathTypes::AbstractType &arg_l, MathTypes::AbstractType &arg_r){
    return arg_l.divide(arg_r);
}

MathTypes::AbstractType &MathOperators::MathOperatorMultiply::Execute(MathTypes::AbstractType &arg_l, MathTypes::AbstractType &arg_r){
    return arg_l.multiply(arg_r);
}

MathTypes::AbstractType &MathOperators::MathOperatorPower::Execute(MathTypes::AbstractType &arg_l, MathTypes::AbstractType &arg_r){
    return arg_l.to_power(arg_r);
}

MathTypes::AbstractType &MathFunctions::MathFunctionSin::Execute(QVector<MathTypes::AbstractType *> & args){
    if ((&args)->size() < this->ArgumentsAmount)
        return *(new MathTypes::TypeDouble(0));
    else
        return args[0]->sin();
}

MathTypes::AbstractType &MathFunctions::MathFunctionCos::Execute(QVector<MathTypes::AbstractType *> & args){
    if ((&args)->size() < this->ArgumentsAmount)
        return *(new MathTypes::TypeDouble(0));
    else
        return args[0]->cos();
}

MathTypes::AbstractType &MathFunctions::MathFunctionTan::Execute(QVector<MathTypes::AbstractType *> & args){
    if ((&args)->size() < this->ArgumentsAmount)
        return *(new MathTypes::TypeDouble(0));
    else
        return args[0]->tg();
}

MathTypes::AbstractType &MathFunctions::MathFunctionCtg::Execute(QVector<MathTypes::AbstractType *> & args){
    if ((&args)->size() < this->ArgumentsAmount)
        return *(new MathTypes::TypeDouble(0));
    else
        return args[0]->ctg();
}

MathTypes::AbstractType &MathFunctions::MathFunctionAbs::Execute(QVector<MathTypes::AbstractType *> &args){
    if ((&args)->size() < this->ArgumentsAmount)
        return *(new MathTypes::TypeDouble(0));
    else
        return args[0]->abs();
}

MathTypes::AbstractType &MathFunctions::MathFunctionLog::Execute(QVector<MathTypes::AbstractType *> &args){
    if ((&args)->size() < this->ArgumentsAmount)
        return *(new MathTypes::TypeDouble(0));
    else
        return args[0]->log();
}

MathTypes::AbstractType &MathFunctions::MathFunctionExp::Execute(QVector<MathTypes::AbstractType *> &args){
    if ((&args)->size() < this->ArgumentsAmount)
        return *(new MathTypes::TypeDouble(0));
    else
        return args[0]->exp();
}
