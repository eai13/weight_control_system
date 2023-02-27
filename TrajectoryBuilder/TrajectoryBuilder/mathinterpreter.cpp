#include "mathinterpreter.h"

MathInterpreter::MathInterpreter(){
    this->Functions["SIN"]      = new MathFunctions::MathFunctionSin();
    this->Functions["COS"]      = new MathFunctions::MathFunctionCos();
    this->Functions["TAN"]      = new MathFunctions::MathFunctionTan();
    this->Functions["CTG"]      = new MathFunctions::MathFunctionCtg();
    this->Functions["ABS"]      = new MathFunctions::MathFunctionAbs();
    this->Functions["EXP"]      = new MathFunctions::MathFunctionExp();
    this->Functions["LOG"]      = new MathFunctions::MathFunctionLog();
    this->Functions["RANGE"]    = new MathFunctions::MathFunctionRange();

    this->Operators["="] = new MathOperators::MathOperatorAssign();
    this->Operators["+"] = new MathOperators::MathOperatorSum();
    this->Operators["-"] = new MathOperators::MathOperatorSubtract();
    this->Operators["*"] = new MathOperators::MathOperatorMultiply();
    this->Operators["/"] = new MathOperators::MathOperatorDivision();
    this->Operators["^"] = new MathOperators::MathOperatorPower();

    this->Variables["X"] = new MathTypes::TypeDouble(5);
    this->Variables["Y"] = new MathTypes::TypeDouble(2);

    qDebug() << "X is " << dynamic_cast<MathTypes::TypeDouble const *>(this->Variables["X"])->GetValue();
    qDebug() << "Y is " << dynamic_cast<MathTypes::TypeDouble const *>(this->Variables["Y"])->GetValue();

    MathTypes::AbstractType & sum_result = this->Operators["^"]->Execute(*(this->Variables["X"]), *(this->Variables["Y"]));
    qDebug() << "Result of sum " << dynamic_cast<MathTypes::TypeDouble const *>(&sum_result)->GetValue();
    MathTypes::AbstractType & assign_result = this->Operators["="]->Execute(*(this->Variables["X"]), *(this->Variables["Y"]));
    qDebug() << "Result of assign " << dynamic_cast<MathTypes::TypeDouble const *>(&assign_result)->GetValue();
    qDebug() << "X is " << dynamic_cast<MathTypes::TypeDouble const *>(this->Variables["X"])->GetValue();
    qDebug() << "Y is " << dynamic_cast<MathTypes::TypeDouble const *>(this->Variables["Y"])->GetValue();

    QVector<MathTypes::AbstractType *> range_args;
    range_args.push_back(new MathTypes::TypeDouble(0));
    range_args.push_back(new MathTypes::TypeDouble(1));
    range_args.push_back(new MathTypes::TypeDouble(0.1));
    MathTypes::AbstractType & range_result = this->Functions["RANGE"]->Execute(range_args);
    qDebug() << "Range result " << dynamic_cast<MathTypes::TypeVector const *>(&range_result)->GetRawCopy();
}

void MathInterpreter::InterpretString(QString commString){

}
