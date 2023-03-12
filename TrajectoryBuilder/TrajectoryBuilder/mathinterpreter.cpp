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

MathInterpreter::MathInterpreter(QObject * parent) : QObject(parent){
    this->Functions["SIN"]          = new MathFunctions::MathFunctionSin();
    this->Functions["COS"]          = new MathFunctions::MathFunctionCos();
    this->Functions["TAN"]          = new MathFunctions::MathFunctionTan();
    this->Functions["CTG"]          = new MathFunctions::MathFunctionCtg();
    this->Functions["ABS"]          = new MathFunctions::MathFunctionAbs();
    this->Functions["EXP"]          = new MathFunctions::MathFunctionExp();
    this->Functions["LOG"]          = new MathFunctions::MathFunctionLog();
    this->Functions["RANGE"]        = new MathFunctions::MathFunctionRange();
    this->Functions["PLOT2D"]       = new MathFunctions::MathFunctionPlot2D();
    this->Functions["PLOT3D"]       = new MathFunctions::MathFunctionPlot3D();
    this->Functions["EXPORT3DTRAJ"] = new MathFunctions::MathFunctionExport3DTrajectory();

    this->Operators["="] = new MathOperators::MathOperatorAssign();
    this->Operators["+"] = new MathOperators::MathOperatorSum();
    this->Operators["-"] = new MathOperators::MathOperatorSubtract();
    this->Operators["*"] = new MathOperators::MathOperatorMultiply();
    this->Operators["/"] = new MathOperators::MathOperatorDivision();
    this->Operators["^"] = new MathOperators::MathOperatorPower();

    this->Variables["PI"] = new MathTypes::TypeDouble(3.1415);
}

void MathInterpreter::InterpretString(QString commString){
    if (this->CheckBraces(commString) == false){
        emit this->siInterpreterDebugString("String preprocessor : Check braces");
        return;
    }

    QStringList FunctionalList = this->StringPreprocessor(commString);
    qDebug() << FunctionalList;

    QString DebugString;
    for (auto iter = FunctionalList.begin(); iter != FunctionalList.end(); iter++){
        DebugString.append(*iter + " ");
    }
    emit this->siInterpreterDebugString("String preprocessor : " + DebugString);

    emit this->siInterpreterDebugString("String interpreter started");
    QStringList TreeList = this->InterpretSubstring(FunctionalList);
    qDebug() << TreeList;

    DebugString.clear();
    for (auto iter = TreeList.begin(); iter != TreeList.end(); iter++){
        DebugString.append(*iter + " ");
    }
    emit this->siInterpreterDebugString("String interpreter : " + DebugString);

    MathTreeProcessor::AbstractSubtree * Tree;
    if (TreeList.isEmpty()){
        emit this->siInterpreterDebugString("String interpretation error");
        return;
    }

    emit this->siInterpreterDebugString("Tree preprocessor started");
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
        emit this->siInterpreterDebugString("Tree preprocessor : Unknown '" + CurrentValue + "'");
        return;
    }

    for (auto iter = TreeList.rbegin(); iter != TreeList.rend(); iter++){
        if (this->Operators.contains(*iter)){
            MathTreeProcessor::AbstractSubtree * NewSubtree = Tree->PushFrontSubtree(this->Operators[*iter], *iter);
            while(NewSubtree == nullptr){
                Tree = Tree->GetParent();
                if (Tree == nullptr){
                    emit this->siInterpreterDebugString("Tree preprocessor : No place in Tree for operator '" + *iter + "'");
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
                    emit this->siInterpreterDebugString("Tree preprocessor : No place in Tree for function '" + *iter + "'");
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
                    emit this->siInterpreterDebugString("Tree preprocessor : No place in Tree for variable '" + *iter + "'");
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
                        emit this->siInterpreterDebugString("Tree preprocessor : No place in Tree for tmp-variable '" + *iter + "'");
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
                        emit this->siInterpreterDebugString("Tree preprocessor : No place for new-variable '" + *iter + "'");
                        return;
                    }
                }
                if (Tree->GetName() == "="){
                    MathTreeProcessor::AbstractSubtree * Subtree = Tree->GetChild();
                    if (Subtree == nullptr){
                        emit this->siInterpreterDebugString("Tree preprocessor : Unable to obtain '" + *iter + "' expected type");
                        return;
                    }
                    MathTypes::AbstractType * NewVariable;
                    switch(Subtree->GetReturnType()){
                    case(MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE):{
                        NewVariable = new MathTypes::TypeDouble(0);
                        emit this->siVariableCreated(*iter + " : DOUBLE");
                        emit this->siInterpreterDebugString("Tree preprocessor : Type of new-variable '" + *iter + "' is DOUBLE");
                        break;
                    }
                    case(MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR):{
                        NewVariable = new MathTypes::TypeVector();
                        emit this->siVariableCreated(*iter + " : VECTOR");
                        emit this->siInterpreterDebugString("Tree preprocessor : Type of new-variable '" + *iter + "' is VECTOR");
                        break;
                    }
                    default:{
                        emit this->siInterpreterDebugString("Tree preprocessor : Unable to obtain '" + *iter + "' expected type");
                        return;
                    }
                    }

                    this->Variables[*iter] = NewVariable;
                    Tree = Tree->PushFrontSubtree(this->Variables[*iter], *iter);
                }
                else{
                    emit this->siInterpreterDebugString("Tree preprocessor : Unknown instance '" + *iter + "'");
                    return;
                }
            }
        }
    }

    emit this->siInterpreterDebugString("Tree interpreter started");
    if (Tree->GetRoot()->Compute() == nullptr){
        emit this->siInterpreterDebugString("Tree interpreter : Failure");
        return;
    }
    emit this->siInterpreterDebugString("Tree interpreter finished");


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

MathInterpreter::~MathInterpreter()
{

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
                    else if ((*iter == ",") && (BracesStack.isEmpty())){
                        Output.append(this->InterpretSubstring(Accumulator));
                        Accumulator.clear();
                        continue;
                    }
                    else if (*iter == "("){
                        BracesStack.push(*iter);
                    }
                    Accumulator.push_back(*iter);
                }
                if (iter == Input.end()) break;
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

QStringList MathInterpreter::StringPreprocessor(QString input){
    QStringList FunctionalList;
    QString Accumulator;
    emit this->siInterpreterDebugString("String preprocessor started");
    for (auto iter = input.begin(); iter != input.end(); iter++){
        if (this->Operators.contains(QString(*iter))){
            if (!(Accumulator.isEmpty())){
                FunctionalList.push_back(Accumulator);
                Accumulator.clear();
                FunctionalList.push_back(QString(*iter));
            }
            else{
                if (*iter == '-'){
                    FunctionalList.push_back("-1");
                    FunctionalList.push_back("*");
                }
                else{
                    FunctionalList.push_back(QString(*iter));
                }
            }

        }
        else if (*iter == '"'){
            if (Accumulator.size()){
                FunctionalList.push_back(Accumulator);
                Accumulator.clear();
            }
            Accumulator.push_back(*iter);
            iter++;
            for(; iter != input.end(); iter++){
                Accumulator.push_back(*iter);
                if (*iter == '"'){
                    FunctionalList.push_back(Accumulator);
                    Accumulator.clear();
                    break;
                }
            }
        }
        else if (iter->isSpace()){

        }
        else if (IS_BRACE(*iter)){
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
            else if (IS_CHAR(Accumulator.back()) || IS_NUMBER(Accumulator.back())){
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
        else if (*iter == ','){
            if (Accumulator.size()){
                FunctionalList.push_back(Accumulator);
                Accumulator.clear();
            }
            FunctionalList.push_back(QString(*iter));
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

    return FunctionalList;
}

void MathInterpreter::slShowVariableWindow(QString Name){
    QStringList NameParts = Name.split(" : ");
    if (NameParts.size() != 2) return;
    if (this->Variables.contains(NameParts[0])){
        switch(this->Variables[NameParts[0]]->GetType()){
        case(MathTypes::AbstractType::MATH_VAR_TYPE_DOUBLE):{
            qreal val = dynamic_cast<MathTypes::TypeDouble *>(this->Variables[NameParts[0]])->GetValue();
            QWidget * Window = new QWidget();
            Window->setAttribute(Qt::WA_DeleteOnClose);
            Window->setAttribute(Qt::WA_ShowModal);
            QBoxLayout * Layout = new QBoxLayout(QBoxLayout::TopToBottom);
            QPushButton * Accept = new QPushButton("Accept");
            QLineEdit * Value = new QLineEdit(QString::asprintf("%.2f", val));
            Layout->addWidget(new QLabel(Name));
            Layout->addWidget(Value);
            Layout->addWidget(Accept);
            Window->setLayout(Layout);
            Window->show();
            break;
        }
        case(MathTypes::AbstractType::MATH_VAR_TYPE_VECTOR):{
            QVector<qreal> val = dynamic_cast<MathTypes::TypeVector *>(this->Variables[NameParts[0]])->GetRawCopy();
            QWidget * Window = new QWidget();
            Window->setAttribute(Qt::WA_DeleteOnClose);
            Window->setAttribute(Qt::WA_ShowModal);
            QBoxLayout * Layout = new QBoxLayout(QBoxLayout::TopToBottom);
            QPushButton * Accept = new QPushButton("Accept");
            QTableWidget * Values = new QTableWidget(1, val.size());
            for (int iter = 0; iter < val.size(); iter++){
                Values->setItem(0, iter, new QTableWidgetItem(QString::asprintf("%.2f", val[iter])));
            }
            Layout->addWidget(new QLabel(Name));
            Layout->addWidget(Values);
            Layout->addWidget(Accept);
            Window->setLayout(Layout);
            Window->show();
            break;
        }
        default:{
            break;
        }
        }
    }
}

void MathInterpreter::slVariableRemoved(QString Name){
    QStringList NameParts = Name.split(" : ");
    if (NameParts.size() != 2) return;
    if (this->Variables.contains(NameParts[0])){
        this->Variables.remove(NameParts[0]);
        emit this->siVariableRemoved(Name);
    }
}

void MathInterpreter::slVariableChanged(QString Name, QString Value){
    QStringList NameParts = Name.split(" : ");
    if (NameParts.size() != 2) return;
    if (!(this->Variables.contains(NameParts[0]))) return;
    if (NameParts[1] == "DOUBLE"){
        double d_val = Value.toDouble();
        dynamic_cast<MathTypes::TypeDouble *>(this->Variables[NameParts[0]])->SetValue(d_val);
    }
    else if (NameParts[1] == "VECTOR"){
        QVector<qreal> vec;
        QStringList s_values = Value.split(";");
        for (auto iter = s_values.begin(); iter != s_values.end(); iter++){
            vec.push_back(iter->toDouble());
        }
        dynamic_cast<MathTypes::TypeVector *>(this->Variables[NameParts[0]])->SetValue(vec);
    }
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
