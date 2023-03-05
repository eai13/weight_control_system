#ifndef MATHTREEPROCESSOR_H
#define MATHTREEPROCESSOR_H

#include <QList>
#include <QVector>

#include "mathfunctions.h"
#include "mathtypes.h"

namespace MathTreeProcessor{
    class AbstractSubtree;
}

class MathTreeProcessor::AbstractSubtree{
public:
    AbstractSubtree(MathFunctions::AbstractFunction * AbsFunc, QString Name, AbstractSubtree * parent = nullptr);
    AbstractSubtree(MathOperators::AbstractOperator * AbsOper, QString Name, AbstractSubtree * parent = nullptr);
    AbstractSubtree(MathTypes::AbstractType         * AbsType, QString Name, AbstractSubtree * parent = nullptr);

    AbstractSubtree * PushBackSubtree(MathTypes::AbstractType * AbsType, QString Name);
    AbstractSubtree * PushBackSubtree(MathFunctions::AbstractFunction * AbsFunc, QString Name);
    AbstractSubtree * PushBackSubtree(MathOperators::AbstractOperator * AbsOper, QString Name);

    AbstractSubtree * PushFrontSubtree(MathTypes::AbstractType * AbsType, QString Name);
    AbstractSubtree * PushFrontSubtree(MathFunctions::AbstractFunction * AbsFunc, QString Name);
    AbstractSubtree * PushFrontSubtree(MathOperators::AbstractOperator * AbsOper, QString Name);

    void SetParent(AbstractSubtree * parent);
    AbstractSubtree * GetParent(void);

    AbstractSubtree * GetRoot(void);

    AbstractSubtree * GetChild(uint8_t index);

    QList<AbstractSubtree *>::iterator ChildBegin();
    QList<AbstractSubtree *>::iterator ChildEnd();

    int GetChildSpaceAvailable(void) { return (this->MaxChildren - this->Subtrees.size()); }

    bool IsValid(void);

    MathTypes::AbstractType * Compute(void);

    QString GetName(void) { return this->Name; }

private:
    AbstractSubtree * Parent = nullptr;

    int MaxChildren;
    QList<AbstractSubtree *> Subtrees;

    QString Name;

    MathFunctions::AbstractFunction * Function = nullptr;
    MathOperators::AbstractOperator * Operator = nullptr;
    MathTypes::AbstractType         * Value = nullptr;
};

#endif // MATHTREEPROCESSOR_H
