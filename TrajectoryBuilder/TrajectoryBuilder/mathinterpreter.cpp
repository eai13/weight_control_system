#include "mathinterpreter.h"

#define IS_CHAR(ch) \
    ((((ch) >= 'A') && ((ch) <= 'Z')) || (((ch) >='a') && ((ch) <= 'z')) || ((ch) == '_'))
#define IS_NUMBER(ch) \
    (((ch) >= '0') && ((ch) <= '9'))
#define IS_SEMICOLON(ch) \
    ((ch) == ';')
#define IS_COMMA(ch) \
    ((ch) == ',')
#define IS_DOT(ch) \
    ((ch) == '.')
#define IS_BRACE(ch) \
    (((ch) == ')') || ((ch) == '(') || ((ch) == '[') || ((ch) == ']'))

MathInterpreter::MathInterpreter(void){
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

    MathTreeProcessor::AbstractSubtree * Result = new MathTreeProcessor::AbstractSubtree(this->Functions["ABS"]);
    Result = Result->AddSubtree(this->Operators["-"]);
    Result->AddSubtree(new MathTypes::TypeDouble(1));
    Result->AddSubtree(new MathTypes::TypeDouble(5));

    Result = Result->GetRoot();
    MathTypes::TypeDouble * end = dynamic_cast<MathTypes::TypeDouble *>(Result->Compute());
    qDebug() << end->GetValue();
}

void MathInterpreter::InterpretString(QString commString){
    if (this->CheckBraces(commString) == false) return;

    QStringList FunctionalList;
    QString Accumulator;
    for (auto iter = commString.begin(); iter != commString.end(); iter++){
        if (this->Operators.contains(QString(*iter))){
            if (!(Accumulator.isEmpty())){
                FunctionalList.push_back(Accumulator);
                Accumulator.clear();
            }
            FunctionalList.push_back(QString(*iter));
        }
        else if (iter->isSpace()){
            if (Accumulator.size()){
                FunctionalList.push_back(Accumulator);
                Accumulator.clear();
            }
        }
        else if (IS_BRACE(*iter)/* || IS_COMMA(*iter) || IS_SEMICOLON(*iter)*/){
            if (!(Accumulator.isEmpty())){
                FunctionalList.push_back(Accumulator);
                Accumulator.clear();
            }
            FunctionalList.push_back(QString(*iter));
        }
        else if (IS_CHAR(*iter)){
            if (Accumulator.isEmpty()){
                Accumulator.push_back(*iter);
            }
            else if (IS_CHAR(Accumulator.back())){
                Accumulator.push_back(*iter);
            }
            else{
                Accumulator.clear();
                Accumulator.push_back(*iter);
            }
        }
        else if (IS_NUMBER(*iter)){
            Accumulator.push_back(*iter);
        }
        else if (IS_DOT(*iter)){
            if (!(Accumulator.isEmpty())){
                if (IS_NUMBER(Accumulator.back())){
                    Accumulator.push_back(*iter);
                }
                else{
                    Accumulator.clear();
                }
            }
        }
        else{
            if (Accumulator.size()){
                FunctionalList.push_back(Accumulator);
                Accumulator.clear();
            }
        }
    }
    if (Accumulator.size()){
        FunctionalList.push_back(Accumulator);
    }
    qDebug() << FunctionalList;

    qDebug() << this->InterpretSubstring(FunctionalList);

}


QList<QStringList> MathInterpreter::InterpretSubstring(QStringList Input){
    QList<QStringList> Output;
    QStack<QString> FunctionalStack;
    QStringList Accumulator;

    for (auto iter = Input.begin(); iter != Input.end(); iter++){
        if (this->Operators.contains(*iter)){
            if (FunctionalStack.isEmpty()){
                FunctionalStack.push(*iter);
            }
            else if (this->Operators[*iter]->GetPrio() > this->Operators[FunctionalStack.top()]->GetPrio()){
                FunctionalStack.push(*iter);
            }
            else{
                while(!(FunctionalStack.isEmpty())){
                    if (this->Operators.contains(FunctionalStack.top())){
                        if (this->Operators[FunctionalStack.top()]->GetPrio() > this->Operators[*iter]->GetPrio()){
                            Accumulator.push_back(FunctionalStack.top());
                            FunctionalStack.pop();
                            Output.push_back(Accumulator);
                            Accumulator.clear();
                        }
                        else{
                            FunctionalStack.push(*iter);
                            break;
                        }
                    }
                    else{
                        Accumulator.push_back(FunctionalStack.top());
                        FunctionalStack.pop();
                        Output.push_back(Accumulator);
                        Accumulator.clear();
                    }
                }
            }
        }
        else if (this->Functions.contains(*iter)){
            QString CurrentFunction = *iter;
            QStack<QString> BracesStack;
            iter++;
            if (*iter == "("){
                iter++;
                for( ; iter != Input.end(); iter++){
                    if (*iter == ")"){
                        if (BracesStack.isEmpty()){
                            Output.append(this->InterpretSubstring(Accumulator));
                            Accumulator.clear();
                            break;
                        }
                        else{
                            BracesStack.pop();
                        }
                    }
                    else if (*iter == "("){
                        BracesStack.push(*iter);
                    }
                    Accumulator.push_back(*iter);
                }
            }
            else{
                qDebug() << "Error Function Arguments";
            }
            Accumulator.push_back(CurrentFunction);
            Output.push_back(Accumulator);
            Accumulator.clear();
        }
        else if (*iter == "("){
            QStack<QString> BracesStack;
            iter++;
            for( ; iter != Input.end(); iter++){
                if (*iter == ")"){
                    if (BracesStack.isEmpty()){
                        Output.append(this->InterpretSubstring(Accumulator));
                        Accumulator.clear();
                        break;
                    }
                    else{
                        BracesStack.pop();
                    }
                }
                else if (*iter == "("){
                    BracesStack.push(*iter);
                }
                Accumulator.push_back(*iter);
            }
        }
        else if (*iter == "["){
            QStack<QString> BracesStack;
            iter++;
            for( ; iter != Input.end(); iter++){
                if (*iter == "]"){
                    if (BracesStack.isEmpty()){
                        Output.append(this->InterpretSubstring(Accumulator));
                        Accumulator.clear();
                        break;
                    }
                    else{
                        BracesStack.pop();
                    }
                }
                else if (*iter == "["){
                    BracesStack.push(*iter);
                }
                Accumulator.push_back(*iter);
            }
        }
        else{
            Accumulator.push_back(*iter);
            Output.push_back(Accumulator);
            Accumulator.clear();
        }
    }

    while(!(FunctionalStack.isEmpty())){
        Accumulator.push_back(FunctionalStack.top());
        FunctionalStack.pop();
        Output.push_back(Accumulator);
        Accumulator.clear();
    }

    return Output;
}

bool MathInterpreter::CheckBraces(const QString & commString){
    QStack<QChar> RoundBraces;
    QStack<QChar> SquareBraces;
    QStack<QChar> FigureBraces;

    for (auto ch = commString.begin(); ch != commString.end(); ch++){
        if (*ch == '('){
            RoundBraces.push(*ch);
        }
        else if (*ch == '['){
            SquareBraces.push(*ch);
        }
        else if (*ch == '{'){
            FigureBraces.push(*ch);
        }
        else if (*ch == ')'){
            if (RoundBraces.size())
                RoundBraces.pop();
            else
                return false;
        }
        else if (*ch == ']'){
            if (SquareBraces.size())
                SquareBraces.pop();
            else
                return false;
        }
        else if (*ch == '}'){
            if (FigureBraces.size()) FigureBraces.pop();
            else return false;
        }
    }
    if ((RoundBraces.size()) || (SquareBraces.size()) || (FigureBraces.size()))
        return false;
    else return true;
}
