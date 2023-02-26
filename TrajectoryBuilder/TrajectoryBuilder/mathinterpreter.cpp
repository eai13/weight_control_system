#include "mathinterpreter.h"

MathInterpreter::MathInterpreter(){
    this->Functions["SIN"] = new MathFunctions::MathFunctionSin();
    this->Functions["COS"] = new MathFunctions::MathFunctionCos();
    this->Functions["TAN"] = new MathFunctions::MathFunctionTan();
    this->Functions["CTG"] = new MathFunctions::MathFunctionCtg();
    this->Functions["ABS"] = new MathFunctions::MathFunctionAbs();
    this->Functions["EXP"] = new MathFunctions::MathFunctionExp();
    this->Functions["LOG"] = new MathFunctions::MathFunctionLog();

    this->Operators["+"] = new MathOperators::MathOperatorSum();
    this->Operators["-"] = new MathOperators::MathOperatorSubtract();
    this->Operators["*"] = new MathOperators::MathOperatorMultiply();
    this->Operators["/"] = new MathOperators::MathOperatorDivision();
    this->Operators["^"] = new MathOperators::MathOperatorPower();

    this->Variables["X"] = new MathTypes::TypeDouble(5);
    this->Variables["Y"] = new MathTypes::TypeDouble(2);

    MathTypes::AbstractType & sum_result = this->Operators["-"]->Execute(*(this->Variables["X"]), *(this->Variables["Y"]));
    if (sum_result.GetType() == MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE){
        qDebug() << dynamic_cast<MathTypes::TypeDouble &>(sum_result).GetValue();
    }
    else{
        qDebug() << "Something went wrong";
    }
}

void MathInterpreter::InterpretString(QString commString){

}
