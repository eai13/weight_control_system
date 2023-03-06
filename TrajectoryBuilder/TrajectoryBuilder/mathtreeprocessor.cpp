#include "mathtreeprocessor.h"
#include <QDebug>

MathTreeProcessor::AbstractSubtree::
AbstractSubtree(QString Name, AbstractSubtree * parent){
    this->Function = nullptr;
    this->Operator = nullptr;
    this->Value = nullptr;
    this->Parent = parent;

    this->Name = Name;

    this->MaxChildren = 0;
}

MathTreeProcessor::AbstractSubtree::
AbstractSubtree(MathFunctions::AbstractFunction * AbsFunc, QString Name, AbstractSubtree * parent){
    this->Function = AbsFunc;
    this->Operator = nullptr;
    this->Value = nullptr;
    this->Parent = parent;

    this->Name = Name;

    this->MaxChildren = AbsFunc->GetArgumentsExpected();
}

MathTreeProcessor::AbstractSubtree::
AbstractSubtree(MathOperators::AbstractOperator * AbsOper, QString Name, AbstractSubtree * parent){
    this->Function = nullptr;
    this->Operator = AbsOper;
    this->Value = nullptr;
    this->Parent = parent;

    this->Name = Name;

    this->MaxChildren = 2;
}

MathTreeProcessor::AbstractSubtree::
AbstractSubtree(MathTypes::AbstractType * AbsType, QString Name, AbstractSubtree * parent){
    this->Function = nullptr;
    this->Operator = nullptr;
    this->Value = AbsType;
    this->Parent = parent;

    this->Name = Name;

    this->MaxChildren = 0;
}

MathTreeProcessor::AbstractSubtree * MathTreeProcessor::AbstractSubtree::
PushBackSubtree(QString Name){
    if (this->MaxChildren <= this->Subtrees.size()) return nullptr;
    AbstractSubtree * NewSubtree = new AbstractSubtree(Name, this);
    this->Subtrees.push_back(NewSubtree);
    return NewSubtree;
}

MathTreeProcessor::AbstractSubtree * MathTreeProcessor::AbstractSubtree::
PushBackSubtree(MathTypes::AbstractType * AbsType, QString Name){
    if (this->MaxChildren <= this->Subtrees.size()) return nullptr;
    AbstractSubtree * NewSubtree = new AbstractSubtree(AbsType, Name, this);
    this->Subtrees.push_back(NewSubtree);
    return NewSubtree;
}

MathTreeProcessor::AbstractSubtree * MathTreeProcessor::AbstractSubtree::
PushBackSubtree(MathFunctions::AbstractFunction * AbsFunc, QString Name){
    if (this->MaxChildren <= this->Subtrees.size()) return nullptr;
    AbstractSubtree * NewSubtree = new AbstractSubtree(AbsFunc, Name, this);
    this->Subtrees.push_back(NewSubtree);
    return NewSubtree;
}

MathTreeProcessor::AbstractSubtree * MathTreeProcessor::AbstractSubtree::
PushBackSubtree(MathOperators::AbstractOperator * AbsOper, QString Name){
    if (this->MaxChildren <= this->Subtrees.size()) return nullptr;
    AbstractSubtree * NewSubtree = new AbstractSubtree(AbsOper, Name, this);
    this->Subtrees.push_back(NewSubtree);
    return NewSubtree;
}

MathTreeProcessor::AbstractSubtree * MathTreeProcessor::AbstractSubtree::
PushFrontSubtree(QString Name){
    if (this->MaxChildren <= this->Subtrees.size()) return nullptr;
    AbstractSubtree * NewSubtree = new AbstractSubtree(Name, this);
    this->Subtrees.push_front(NewSubtree);
    return NewSubtree;
}

MathTreeProcessor::AbstractSubtree * MathTreeProcessor::AbstractSubtree::
PushFrontSubtree(MathTypes::AbstractType * AbsType, QString Name){
    if (this->MaxChildren <= this->Subtrees.size()) return nullptr;
    AbstractSubtree * NewSubtree = new AbstractSubtree(AbsType, Name, this);
    this->Subtrees.push_front(NewSubtree);
    return NewSubtree;
}

MathTreeProcessor::AbstractSubtree * MathTreeProcessor::AbstractSubtree::
PushFrontSubtree(MathOperators::AbstractOperator * AbsOper, QString Name){
    if (this->MaxChildren <= this->Subtrees.size()) return nullptr;
    AbstractSubtree * NewSubtree = new AbstractSubtree(AbsOper, Name, this);
    this->Subtrees.push_front(NewSubtree);
    return NewSubtree;
}

MathTreeProcessor::AbstractSubtree * MathTreeProcessor::AbstractSubtree::
PushFrontSubtree(MathFunctions::AbstractFunction * AbsFunc, QString Name){
    if (this->MaxChildren <= this->Subtrees.size()) return nullptr;
    AbstractSubtree * NewSubtree = new AbstractSubtree(AbsFunc, Name, this);
    this->Subtrees.push_front(NewSubtree);
    return NewSubtree;
}

void MathTreeProcessor::AbstractSubtree::SetParent(AbstractSubtree * parent){
    this->Parent = parent;
}

MathTreeProcessor::AbstractSubtree * MathTreeProcessor::AbstractSubtree::
GetParent(void){
    return this->Parent;
}

MathTreeProcessor::AbstractSubtree * MathTreeProcessor::AbstractSubtree::
GetRoot(void){
    if (this->Parent == nullptr)
        return this;
    else
        return this->Parent->GetRoot();
}

MathTreeProcessor::AbstractSubtree * MathTreeProcessor::AbstractSubtree::
GetChild(uint8_t index){
    if (index < this->Subtrees.size())
        return Subtrees.at(index);
    else
        return nullptr;
}

QList<MathTreeProcessor::AbstractSubtree *>::iterator MathTreeProcessor::AbstractSubtree::
ChildBegin(){
    return this->Subtrees.begin();
}

QList<MathTreeProcessor::AbstractSubtree *>::iterator MathTreeProcessor::AbstractSubtree::
ChildEnd(){
    return this->Subtrees.end();
}

bool MathTreeProcessor::AbstractSubtree::
IsValid(void){
    if (this->Function != nullptr){
        if (this->Function->GetArgumentsExpected() != this->Subtrees.size())
            return false;
        else
            return true;
    }
    if (this->Operator != nullptr){
        if (this->Subtrees.size() != 2)
            return false;
        else
            return true;
    }
    if (this->Value != nullptr){
        if (this->Subtrees.size())
            return false;
        else
            return true;
    }
    return false;
}

MathTypes::AbstractType * MathTreeProcessor::AbstractSubtree::
Compute(void){
    if (!(this->IsValid()))
        return nullptr;

    QVector<MathTypes::AbstractType *> Arguments;

    for (auto iter = this->ChildBegin(); iter != this->ChildEnd(); iter++){
        MathTypes::AbstractType * CurrentArg = (*(iter))->Compute();
        if (CurrentArg == nullptr)
            return nullptr;
        else
            Arguments.push_back(CurrentArg);
    }

    if (this->Operator != nullptr){
        this->Value = this->Operator->Execute(Arguments.at(0), Arguments.at(1));
    }
    else if (this->Function != nullptr){
        this->Value = this->Function->Execute(&Arguments);
    }
    else if (this->Value != nullptr){

    }
    else{
        this->Value = new MathTypes::TypeDouble(0);
    }

    return this->Value;
}

MathTypes::AbstractType::VarTypes MathTreeProcessor::AbstractSubtree::
GetReturnType(void){
    if (this->ReturnType != MathTypes::AbstractType::MATH_VAR_TYPE_NONE){
        return this->ReturnType;
    }

    QVector<MathTypes::AbstractType::VarTypes> Arguments;
    if (this->Function != nullptr){
        for (auto iter = this->ChildBegin(); iter != this->ChildEnd(); iter++){
            Arguments.push_back((*iter)->GetReturnType());
        }
        this->ReturnType = this->Function->GetReturnValue(Arguments);
        return this->ReturnType;
    }
    else if (this->Operator != nullptr){
        for (auto iter = this->ChildBegin(); iter != this->ChildEnd(); iter++){
            Arguments.push_back((*iter)->GetReturnType());
        }
        this->ReturnType = this->Operator->GetReturnValue(Arguments[0], Arguments[1]);
        return this->ReturnType;
    }
    else if (this->Value != nullptr){
        this->ReturnType = this->Value->GetType();
        return this->ReturnType;
    }
    else{
        return MathTypes::AbstractType::MATH_VAR_TYPE_NONE;
    }
}

