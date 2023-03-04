#include "mathfunctions.h"
#include <QVector>


MathTypes::AbstractType * MathOperators::MathOperatorAssign::Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    return arg_l->assign(arg_r);
}

MathTypes::AbstractType * MathOperators::MathOperatorSum::Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    return arg_l->sum(arg_r);
}

MathTypes::AbstractType * MathOperators::MathOperatorSubtract::Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    return arg_l->subtract(arg_r);
}

MathTypes::AbstractType * MathOperators::MathOperatorDivision::Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    return arg_l->divide(arg_r);
}

MathTypes::AbstractType * MathOperators::MathOperatorMultiply::Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    return arg_l->multiply(arg_r);
}

MathTypes::AbstractType * MathOperators::MathOperatorPower::Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    return arg_l->to_power(arg_r);
}

MathTypes::AbstractType * MathFunctions::MathFunctionSin::Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeDouble(0);
    else
        return args->at(0)->sin();
}

MathTypes::AbstractType * MathFunctions::MathFunctionCos::Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeDouble(0);
    else
        return args->at(0)->cos();
}

MathTypes::AbstractType * MathFunctions::MathFunctionTan::Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeDouble(0);
    else
        return args->at(0)->tg();
}

MathTypes::AbstractType * MathFunctions::MathFunctionCtg::Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeDouble(0);
    else
        return args->at(0)->ctg();
}

MathTypes::AbstractType * MathFunctions::MathFunctionAbs::Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeDouble(0);
    else
        return args->at(0)->abs();
}

MathTypes::AbstractType * MathFunctions::MathFunctionLog::Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeDouble(0);
    else
        return args->at(0)->log();
}

MathTypes::AbstractType * MathFunctions::MathFunctionExp::Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeDouble(0);
    else
        return args->at(0)->exp();
}

MathTypes::AbstractType * MathFunctions::MathFunctionRange::Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeVector(QVector<qreal>(0));
    else{
        qreal start = (dynamic_cast<MathTypes::TypeDouble *>(args->at(0))->GetValue());
        qreal end = (dynamic_cast<MathTypes::TypeDouble *>(args->at(1))->GetValue());
        qreal step = (dynamic_cast<MathTypes::TypeDouble *>(args->at(2))->GetValue());

        QVector<qreal> r_vec;
        for (; start < end; start += step)
            r_vec.push_back(start);

        MathTypes::TypeVector * ret_vec = new MathTypes::TypeVector(r_vec);
        return ret_vec;
    }
}

uint8_t MathFunctions::AbstractFunction::GetArgumentsExpected(void){
    return this->ArgumentsAmount;
}