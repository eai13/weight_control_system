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

    enum OperatorPriority{
        ASSIGN_PRIO     = 0x00,
        SUM_PRIO        = 0x01,
        SUBTRACT_PRIO   = 0x01,
        MULTIPLY_PRIO   = 0x02,
        DIVISION_PRIO   = 0x02,
        POWER_PRIO      = 0x03,
        FUNCTION_PRIO   = 0xFF
    };
}

class MathOperators::AbstractOperator{
public:
    virtual ~AbstractOperator(void) {}
    virtual MathTypes::AbstractType * Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r) = 0;
    virtual MathTypes::AbstractType::VarTypes GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r) = 0;

    OperatorPriority GetPrio(void){
        return this->Priority;
    }

protected:
    AbstractOperator(void) {}
    OperatorPriority Priority;
};

class MathOperators::MathOperatorAssign : public AbstractOperator {
public:
    MathOperatorAssign(void) { this->Priority = ASSIGN_PRIO; }

    MathTypes::AbstractType * Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r) override;
    MathTypes::AbstractType::VarTypes GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r) override;
    ~MathOperatorAssign(void) override {}
};

class MathOperators::MathOperatorSum : public AbstractOperator {
public:
    MathOperatorSum(void) { this->Priority = SUM_PRIO; }

    MathTypes::AbstractType * Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r) override;
    MathTypes::AbstractType::VarTypes GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r) override;
    ~MathOperatorSum(void) override {}
};

class MathOperators::MathOperatorSubtract : public AbstractOperator {
public:
    MathOperatorSubtract(void) { this->Priority = SUBTRACT_PRIO; }

    MathTypes::AbstractType * Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r) override;
    MathTypes::AbstractType::VarTypes GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r) override;
    ~MathOperatorSubtract(void) override {}
};

class MathOperators::MathOperatorDivision : public AbstractOperator {
public:
    MathOperatorDivision(void) { this->Priority = DIVISION_PRIO; }

    MathTypes::AbstractType * Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r) override;
    MathTypes::AbstractType::VarTypes GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r) override;
    ~MathOperatorDivision(void) override {}
};

class MathOperators::MathOperatorMultiply : public AbstractOperator {
public:
    MathOperatorMultiply(void) { this->Priority = MULTIPLY_PRIO; }

    MathTypes::AbstractType * Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r) override;
    MathTypes::AbstractType::VarTypes GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r) override;
    ~MathOperatorMultiply(void) override {}
};

class MathOperators::MathOperatorPower : public AbstractOperator {
public:
    MathOperatorPower(void) { this->Priority = POWER_PRIO; }

    MathTypes::AbstractType * Execute(MathTypes::AbstractType * arg_l, MathTypes::AbstractType * arg_r) override;
    MathTypes::AbstractType::VarTypes GetReturnValue(MathTypes::AbstractType::VarTypes arg_type_l, MathTypes::AbstractType::VarTypes arg_type_r) override;
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
    virtual MathTypes::AbstractType * Execute(QVector<MathTypes::AbstractType *> * args) = 0;
    virtual MathTypes::AbstractType::VarTypes GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args) = 0;

    uint8_t GetArgumentsExpected(void);

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
    uint8_t ArgumentsAmount = 0;
    AbstractFunction(void) {}
};

class MathFunctions::MathFunctionSin : public AbstractFunction {
public:
    MathFunctionSin(void) { this->ArgumentsAmount = SIN_ARG_AM; }

    MathTypes::AbstractType * Execute(QVector<MathTypes::AbstractType *> * args) override;
    MathTypes::AbstractType::VarTypes GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args) override;
    ~MathFunctionSin(void) override {}
};

class MathFunctions::MathFunctionCos : public AbstractFunction {
public:
    MathFunctionCos(void) { this->ArgumentsAmount = COS_ARG_AM; }

    MathTypes::AbstractType * Execute(QVector<MathTypes::AbstractType *> * args) override;
    MathTypes::AbstractType::VarTypes GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args) override;
    ~MathFunctionCos(void) override {}
};

class MathFunctions::MathFunctionTan : public AbstractFunction {
public:
    MathFunctionTan(void) { this->ArgumentsAmount = TAN_ARG_AM; }

    MathTypes::AbstractType * Execute(QVector<MathTypes::AbstractType *> * args) override;
    MathTypes::AbstractType::VarTypes GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args) override;
    ~MathFunctionTan(void) override {}
};

class MathFunctions::MathFunctionCtg : public AbstractFunction {
public:
    MathFunctionCtg(void) { this->ArgumentsAmount = CTG_ARG_AM; }

    MathTypes::AbstractType * Execute(QVector<MathTypes::AbstractType *> * args) override;
    MathTypes::AbstractType::VarTypes GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args) override;
    ~MathFunctionCtg(void) override {}
};

class MathFunctions::MathFunctionAbs : public AbstractFunction {
public:
    MathFunctionAbs(void) { this->ArgumentsAmount = ABS_ARG_AM; }

    MathTypes::AbstractType * Execute(QVector<MathTypes::AbstractType *> * args) override;
    MathTypes::AbstractType::VarTypes GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args) override;
    ~MathFunctionAbs(void) override {}
};

class MathFunctions::MathFunctionLog : public AbstractFunction {
public:
    MathFunctionLog(void) { this->ArgumentsAmount = LOG_ARG_AM; }

    MathTypes::AbstractType * Execute(QVector<MathTypes::AbstractType *> * args) override;
    MathTypes::AbstractType::VarTypes GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args) override;
    ~MathFunctionLog(void) override {}
};

class MathFunctions::MathFunctionExp : public AbstractFunction {
public:
    MathFunctionExp(void) { this->ArgumentsAmount = EXP_ARG_AM; }

    MathTypes::AbstractType * Execute(QVector<MathTypes::AbstractType *> * args) override;
    MathTypes::AbstractType::VarTypes GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args) override;
    ~MathFunctionExp(void) override {}
};

class MathFunctions::MathFunctionRange : public AbstractFunction {
public:
    MathFunctionRange(void) { this->ArgumentsAmount = RANGE_ARG_AM; }

    MathTypes::AbstractType * Execute(QVector<MathTypes::AbstractType *> * args) override;
    MathTypes::AbstractType::VarTypes GetReturnValue(QVector<MathTypes::AbstractType::VarTypes> args) override;
    ~MathFunctionRange(void) override {}
};

#endif // MATHFUNCTIONS_H
