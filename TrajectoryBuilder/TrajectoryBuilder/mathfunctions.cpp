#include "mathfunctions.h"
#include <QVector>


MathTypes::AbstractType * MathOperators::MathOperatorAssign::
Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    return arg_l->assign(arg_r);
}

MathTypes::AbstractType::VarTypes MathOperators::MathOperatorAssign::
GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r){
    if (arg_type_l == arg_type_r) return arg_type_l;
    else return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
}

MathTypes::AbstractType * MathOperators::MathOperatorSum::
Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    return arg_l->sum(arg_r);
}

MathTypes::AbstractType::VarTypes MathOperators::MathOperatorSum::
GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r){
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_NONE) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_NONE)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR;
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE;
    }
}

MathTypes::AbstractType * MathOperators::MathOperatorSubtract::
Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    return arg_l->subtract(arg_r);
}

MathTypes::AbstractType::VarTypes MathOperators::MathOperatorSubtract::
GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r){
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_NONE) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_NONE)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR;
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE;
    }
}

MathTypes::AbstractType * MathOperators::MathOperatorDivision::
Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    return arg_l->divide(arg_r);
}

MathTypes::AbstractType::VarTypes MathOperators::MathOperatorDivision::
GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r){
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_NONE) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_NONE)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR;
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE;
    }
}

MathTypes::AbstractType * MathOperators::MathOperatorMultiply::
Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    return arg_l->multiply(arg_r);
}

MathTypes::AbstractType::VarTypes MathOperators::MathOperatorMultiply::
GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r){
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_NONE) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_NONE)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR;
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE;
    }
}

MathTypes::AbstractType * MathOperators::MathOperatorPower::
Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r){
    return arg_l->to_power(arg_r);
}

MathTypes::AbstractType::VarTypes MathOperators::MathOperatorPower::
GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r){
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_NONE) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_NONE)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
    if ((arg_type_l == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR) ||
        (arg_type_r == MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR)){
        return MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR;
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionSin::
Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeDouble(0);
    else
        return args->at(0)->sin();
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionSin::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (args.size()){
        return args[0];
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionCos::Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeDouble(0);
    else
        return args->at(0)->cos();
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionCos::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (args.size()){
        return args[0];
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionTan::
Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeDouble(0);
    else
        return args->at(0)->tg();
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionTan::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (args.size()){
        return args[0];
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionCtg::
Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeDouble(0);
    else
        return args->at(0)->ctg();
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionCtg::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (args.size()){
        return args[0];
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionAbs::
Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeDouble(0);
    else
        return args->at(0)->abs();
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionAbs::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (args.size()){
        return args[0];
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionLog::
Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeDouble(0);
    else
        return args->at(0)->log();
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionLog::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (args.size()){
        return args[0];
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionExp::
Execute(QVector<MathTypes::AbstractType *> * args){
    if (args->size() < this->ArgumentsAmount)
        return new MathTypes::TypeDouble(0);
    else
        return args->at(0)->exp();
}

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionExp::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (args.size()){
        return args[0];
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

MathTypes::AbstractType * MathFunctions::MathFunctionRange::
Execute(QVector<MathTypes::AbstractType *> * args){
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

MathTypes::AbstractType::VarTypes MathFunctions::MathFunctionRange::
GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args){
    if (this->ArgumentsAmount <= args.size()){
        if ((args[0] == MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE) &&
            (args[1] == MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE) &&
            (args[2] == MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE)){
            return MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR;
        }
        else{
            return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
        }
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

uint8_t MathFunctions::AbstractFunction::
GetArgumentsExpected(void){
    return this->ArgumentsAmount;
}
