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

    MathTreeProcessor::AbstractSubtree * Result = new MathTreeProcessor::AbstractSubtree(this->Functions["ABS"], "ABS");
    Result = Result->PushBackSubtree(this->Operators["-"], "-");
    Result->PushBackSubtree(new MathTypes::TypeDouble(1), "1");
    Result->PushBackSubtree(new MathTypes::TypeDouble(5), "5");

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

    QStringList TreeList = this->InterpretSubstring(FunctionalList);
    qDebug() << TreeList;

    MathTreeProcessor::AbstractSubtree * Tree;
    if (TreeList.isEmpty()) return;
    QString CurrentValue = TreeList.takeLast();
    if (this->Operators.contains(CurrentValue)){
        Tree = new MathTreeProcessor::AbstractSubtree(this->Operators[CurrentValue], CurrentValue);
    }
    else if (this->Functions.contains(CurrentValue)){
        Tree = new MathTreeProcessor::AbstractSubtree(this->Functions[CurrentValue], CurrentValue);
    }
    else if (this->Variables.contains(CurrentValue)){
        Tree = new MathTreeProcessor::AbstractSubtree(this->Variables[CurrentValue], CurrentValue);
    }
    else{
        qDebug() << "Something wrong, quit";
        return;
    }

    for (auto iter = TreeList.rbegin(); iter != TreeList.rend(); iter++){
        if (this->Operators.contains(*iter)){
            MathTreeProcessor::AbstractSubtree * NewSubtree = Tree->PushFrontSubtree(this->Operators[*iter], *iter);
            while(NewSubtree == nullptr){
                Tree = Tree->GetParent();
                if (Tree == nullptr){
                    qDebug() << "Error, no place for an Operator";
                    return;
                }
                NewSubtree = Tree->PushFrontSubtree(this->Operators[*iter], *iter);
            }
            Tree = NewSubtree;
        }
        else if (this->Functions.contains(*iter)){
            MathTreeProcessor::AbstractSubtree * NewSubtree = Tree->PushFrontSubtree(this->Functions[*iter], *iter);
            while(NewSubtree == nullptr){
                Tree = Tree->GetParent();
                if (Tree == nullptr){
                    qDebug() << "Error, no place for a Function";
                    return;
                }
                NewSubtree = Tree->PushFrontSubtree(this->Functions[*iter], *iter);
            }
            Tree = NewSubtree;
        }
        else if (this->Variables.contains(*iter)){
            MathTreeProcessor::AbstractSubtree * NewSubtree = Tree->PushFrontSubtree(this->Variables[*iter], *iter);
            while(NewSubtree == nullptr){
                Tree = Tree->GetParent();
                if (Tree == nullptr){
                    qDebug() << "Error, no place for a Variable";
                    return;
                }
                NewSubtree = Tree->PushFrontSubtree(this->Variables[*iter], *iter);
            }
            Tree = NewSubtree;
        }
        else{
            bool isNumber;
            iter->toDouble(&isNumber);
            if (isNumber){
                MathTypes::AbstractType * NewNumber = new MathTypes::TypeDouble(iter->toDouble());
                MathTreeProcessor::AbstractSubtree * NewSubtree = Tree->PushFrontSubtree(NewNumber, *iter);
                while(NewSubtree == nullptr){
                    Tree = Tree->GetParent();
                    if (Tree == nullptr){
                        qDebug() << "Error, no place for a Number";
                        return;
                    }
                    NewSubtree = Tree->PushFrontSubtree(NewNumber, *iter);
                }
                Tree = NewSubtree;
            }
            else{
                while(Tree->GetChildSpaceAvailable() == 0){
                    Tree = Tree->GetParent();
                    if (Tree == nullptr){
                        qDebug() << "Error, no place for an unknown variable";
                        return;
                    }
                }
                if (Tree->GetName() == "="){
                    MathTreeProcessor::AbstractSubtree * Subtree = Tree->GetChild();
                    if (Subtree == nullptr){
                        qDebug() << "Unable to detect new variable '" << *iter << "' expected type";
                        return;
                    }
                    MathTypes::AbstractType * NewVariable;
                    switch(Subtree->GetReturnType()){
                    case(MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE):{
                        NewVariable = new MathTypes::TypeDouble(0);
                        qDebug() << "New variable '" << *iter << "' type is DOUBLE";
                        break;
                    }
                    case(MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR):{
                        NewVariable = new MathTypes::TypeVector();
                        qDebug() << "New variable '" << *iter << "' type is VECTOR";
                        break;
                    }
                    default:{
                        qDebug() << "Unable to detect new variable '" << *iter << "' expected type";
                        return;
                    }
                    }

                    this->Variables[*iter] = NewVariable;
                    Tree = Tree->PushFrontSubtree(this->Variables[*iter], *iter);
                }
                else{
                    qDebug() << "Unknown variable " << *iter;
                    return;
                }
            }
        }
    }

    Tree->GetRoot()->Compute();

    qDebug() << "Actual variables :";
    for (auto iter = this->Variables.begin(); iter != Variables.end(); iter++){
        switch(iter.value()->GetType()){
        case(MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE):{
            qDebug() << "\t" << iter.key() << " : " << dynamic_cast<MathTypes::TypeDouble *>(iter.value())->GetValue();
            break;
        }
        case(MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR):{
            qDebug() << "\t" << iter.key() << " : " << dynamic_cast<MathTypes::TypeVector *>(iter.value())->GetRawCopy();
            break;
        }
        case(MathTypes::AbstractType::MATH_VAR_TYPE_NONE):{
            //NONE
            break;
        }
        default:{
            //NONE
            break;
        }
        }
    }
}


QStringList MathInterpreter::InterpretSubstring(QStringList Input){
    QStringList Output;
    QStack<QString> FunctionalStack;
    QStringList Accumulator;

    for (auto iter = Input.begin(); iter != Input.end(); iter++){
        // If an Operator
        if (this->Operators.contains(*iter)){
            if (FunctionalStack.isEmpty()){
                FunctionalStack.push(*iter);
            }
            else if (this->Operators[*iter]->GetPrio() > this->Operators[FunctionalStack.top()]->GetPrio()){
                FunctionalStack.push(*iter);
            }
            else{
                while(FunctionalStack.size()){
                    if (this->Operators.contains(FunctionalStack.top())){
                        if (this->Operators[FunctionalStack.top()]->GetPrio() > this->Operators[*iter]->GetPrio()){
                            Output.push_back(FunctionalStack.top());
                            FunctionalStack.pop();
                        }
                        else{
                            FunctionalStack.push(*iter);
                            break;
                        }
                    }
                    else{
                        Output.push_back(FunctionalStack.top());
                        FunctionalStack.pop();
                    }
                }
                if (FunctionalStack.isEmpty()){
                    FunctionalStack.push(*iter);
                }
            }
        }
        // If a Function
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
            Output.append(CurrentFunction);
            Accumulator.clear();
        }
        // If in Round Braces
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
        // If in Square Braces
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
            Output.push_back(*iter);
            Accumulator.clear();
        }
    }

    while(!(FunctionalStack.isEmpty())){
        Output.append(FunctionalStack.top());
        FunctionalStack.pop();
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
