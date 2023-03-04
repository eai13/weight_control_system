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

    MathTreeProcessor::AbstractSubtree * Result = new MathTreeProcessor::AbstractSubtree(this->Functions["RANGE"]);
    Result->AddSubtree(new MathTypes::TypeDouble(0));
    Result->AddSubtree(new MathTypes::TypeDouble(1));
    Result->AddSubtree(new MathTypes::TypeDouble(0.1));

    MathTypes::TypeVector * end = dynamic_cast<MathTypes::TypeVector *>(Result->Compute());
    qDebug() << end->GetRawCopy();
}

void MathInterpreter::InterpretString(QString commString){

}

uint32_t MathInterpreter::CheckBraces(const QString & commString){
    QStack<QChar> RoundBraces;
    QStack<QChar> SquareBraces;
    QStack<QChar> FigureBraces;

//    for (auto char_iter = commString.begin(); char_iter != commString.end(); char_iter++)
}