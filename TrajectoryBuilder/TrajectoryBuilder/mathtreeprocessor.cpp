#include "mathtreeprocessor.h"

MathTreeProcessor::AbstractSubtree::
AbstractSubtree(MathFunctions::AbstractFunction * AbsFunc, AbstractSubtree * parent){
    this->Function = AbsFunc;
    this->Operator = nullptr;
    this->Value = nullptr;
    this->Parent = parent;
}

MathTreeProcessor::AbstractSubtree::
AbstractSubtree(MathOperators::AbstractOperator * AbsOper, AbstractSubtree * parent){
    this->Function = nullptr;
    this->Operator = AbsOper;
    this->Value = nullptr;
    this->Parent = parent;
}

MathTreeProcessor::AbstractSubtree::
AbstractSubtree(MathTypes::AbstractType * AbsType, AbstractSubtree * parent){
    this->Function = nullptr;
    this->Operator = nullptr;
    this->Value = AbsType;
    this->Parent = parent;
}

template<typename T>
MathTreeProcessor::AbstractSubtree * MathTreeProcessor::AbstractSubtree::
AddSubtree(T * AbsFunc){
    AbstractSubtree * NewSubtree = new AbstractSubtree(AbsFunc, this);
    this->Subtrees.push_back(NewSubtree);
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
        return this->Operator->Execute(Arguments.at(0), Arguments.at(1));
    }
}

