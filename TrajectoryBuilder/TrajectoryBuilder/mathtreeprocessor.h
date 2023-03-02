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
    AbstractSubtree(MathFunctions::AbstractFunction * AbsFunc, AbstractSubtree * parent = nullptr);
    AbstractSubtree(MathOperators::AbstractOperator * AbsOper, AbstractSubtree * parent = nullptr);
    AbstractSubtree(MathTypes::AbstractType         * AbsType, AbstractSubtree * parent = nullptr);

    template<typename T>
    AbstractSubtree * AddSubtree(T * AbsFunc);

    void SetParent(AbstractSubtree * parent);
    AbstractSubtree * GetParent(void);

    AbstractSubtree * GetChild(uint8_t index);

    QList<AbstractSubtree *>::iterator ChildBegin();
    QList<AbstractSubtree *>::iterator ChildEnd();

    bool IsValid(void);

    MathTypes::AbstractType * Compute(void);

private:
    AbstractSubtree * Parent = nullptr;
    QList<AbstractSubtree *> Subtrees;

    MathFunctions::AbstractFunction * Function = nullptr;
    MathOperators::AbstractOperator * Operator = nullptr;
    MathTypes::AbstractType         * Value = nullptr;
};

#endif // MATHTREEPROCESSOR_H
