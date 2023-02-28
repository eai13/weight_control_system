#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

#include <QString>
#include <QMap>
#include <QtMath>
#include "mathtypes.h"
#include <QVector>

namespace MathOperators{
    class AbstractOperator;
    class MathOperatorAssign;
    class MathOperatorSum;
    class MathOperatorSubtract;
    class MathOperatorMultiply;
    class MathOperatorDivision;
    class MathOperatorPower;
}

class MathOperators::AbstractOperator{
public:
    virtual ~AbstractOperator(void) {}
    virtual MathTypes::AbstractType & Execute(MathTypes::AbstractType & arg_l, MathTypes::AbstractType & arg_r) = 0;

protected:
    AbstractOperator(void) {}
};

class MathOperators::MathOperatorAssign : public AbstractOperator {
public:
    MathOperatorAssign(void) {}

    MathTypes::AbstractType & Execute(MathTypes::AbstractType & arg_l, MathTypes::AbstractType & arg_r) override;
    ~MathOperatorAssign(void) override {}
};

class MathOperators::MathOperatorSum : public AbstractOperator {
public:
    MathOperatorSum(void) {}

    MathTypes::AbstractType & Execute(MathTypes::AbstractType & arg_l, MathTypes::AbstractType & arg_r) override;
    ~MathOperatorSum(void) override {}
};

class MathOperators::MathOperatorSubtract : public AbstractOperator {
public:
    MathOperatorSubtract(void) {}

    MathTypes::AbstractType & Execute(MathTypes::AbstractType & arg_l, MathTypes::AbstractType & arg_r) override;
    ~MathOperatorSubtract(void) override {}
};

class MathOperators::MathOperatorDivision : public AbstractOperator {
public:
    MathOperatorDivision(void) {}

    MathTypes::AbstractType & Execute(MathTypes::AbstractType & arg_l, MathTypes::AbstractType & arg_r) override;
    ~MathOperatorDivision(void) override {}
};

class MathOperators::MathOperatorMultiply : public AbstractOperator {
public:
    MathOperatorMultiply(void) {}

    MathTypes::AbstractType & Execute(MathTypes::AbstractType & arg_l, MathTypes::AbstractType & arg_r) override;
    ~MathOperatorMultiply(void) override {}
};

class MathOperators::MathOperatorPower : public AbstractOperator {
public:
    MathOperatorPower(void) {}

    MathTypes::AbstractType & Execute(MathTypes::AbstractType & arg_l, MathTypes::AbstractType & arg_r) override;
    ~MathOperatorPower(void) override {}
};

namespace MathFunctions{
    class AbstractFunction;
    class MathFunctionSin;
    class MathFunctionCos;
    class MathFunctionTan;
    class MathFunctionCtg;
    class MathFunctionAbs;
    class MathFunctionLog;
    class MathFunctionExp;
    class MathFunctionRange;
}

class MathFunctions::AbstractFunction{
public:
    virtual ~AbstractFunction(void) {}
    virtual MathTypes::AbstractType & Execute(QVector<MathTypes::AbstractType *> & args) = 0;

protected:
    enum FunctionArgsAmount{
        SIN_ARG_AM      = 1,
        COS_ARG_AM      = 1,
        TAN_ARG_AM      = 1,
        CTG_ARG_AM      = 1,
        ABS_ARG_AM      = 1,
        LOG_ARG_AM      = 1,
        EXP_ARG_AM      = 1,
        RANGE_ARG_AM    = 3
    };
    int ArgumentsAmount;
    AbstractFunction(void) {}
};

class MathFunctions::MathFunctionSin : public AbstractFunction {
public:
    MathFunctionSin(void) { this->ArgumentsAmount = SIN_ARG_AM; }

    MathTypes::AbstractType & Execute(QVector<MathTypes::AbstractType *> & args) override;
    ~MathFunctionSin(void) override {}
};

class MathFunctions::MathFunctionCos : public AbstractFunction {
public:
    MathFunctionCos(void) { this->ArgumentsAmount = COS_ARG_AM; }

    MathTypes::AbstractType & Execute(QVector<MathTypes::AbstractType *> & args) override;
    ~MathFunctionCos(void) override {}
};

class MathFunctions::MathFunctionTan : public AbstractFunction {
public:
    MathFunctionTan(void) { this->ArgumentsAmount = TAN_ARG_AM; }

    MathTypes::AbstractType & Execute(QVector<MathTypes::AbstractType *> & args) override;
    ~MathFunctionTan(void) override {}
};

class MathFunctions::MathFunctionCtg : public AbstractFunction {
public:
    MathFunctionCtg(void) { this->ArgumentsAmount = CTG_ARG_AM; }

    MathTypes::AbstractType & Execute(QVector<MathTypes::AbstractType *> & args) override;
    ~MathFunctionCtg(void) override {}
};

class MathFunctions::MathFunctionAbs : public AbstractFunction {
public:
    MathFunctionAbs(void) { this->ArgumentsAmount = ABS_ARG_AM; }

    MathTypes::AbstractType & Execute(QVector<MathTypes::AbstractType *> & args) override;
    ~MathFunctionAbs(void) override {}
};

class MathFunctions::MathFunctionLog : public AbstractFunction {
public:
    MathFunctionLog(void) { this->ArgumentsAmount = LOG_ARG_AM; }

    MathTypes::AbstractType & Execute(QVector<MathTypes::AbstractType *> & args) override;
    ~MathFunctionLog(void) override {}
};

class MathFunctions::MathFunctionExp : public AbstractFunction {
public:
    MathFunctionExp(void) { this->ArgumentsAmount = EXP_ARG_AM; }

    MathTypes::AbstractType & Execute(QVector<MathTypes::AbstractType *> & args) override;
    ~MathFunctionExp(void) override {}
};

class MathFunctions::MathFunctionRange : public AbstractFunction {
public:
    MathFunctionRange(void) { this->ArgumentsAmount = RANGE_ARG_AM; }

    MathTypes::AbstractType & Execute(QVector<MathTypes::AbstractType *> & args) override;
    ~MathFunctionRange(void) override {}
};

#endif // MATHFUNCTIONS_H
