#ifndef MATHTYPES_H
#define MATHTYPES_H

#include <QString>
#include <QVector>
#include <QtMath>
#include <math.h>
#include <QObject>

namespace MathTypes{
    class AbstractType;
    class TypeVector;
    class TypeDouble;
};

/***
 *  ABSTRACT Type
 */

class MathTypes::AbstractType{
public:
    enum VarTypes{
        MATH_VAR_TYPE_NONE,
        MATH_VAR_TYPE_DOUBLE,
        MATH_VAR_TYPE_VECTOR
    };

    virtual ~AbstractType(void) {}
    virtual VarTypes GetType(void) const{
        return MATH_VAR_TYPE_NONE;
    }

    /***
     * @brief Operators
     */
    virtual AbstractType * assign(AbstractType const * r_value) = 0;
    virtual AbstractType * sum(AbstractType const * r_value) const = 0;
    virtual AbstractType * subtract(AbstractType const * r_value) const = 0;
    virtual AbstractType * divide(AbstractType const * r_value) const = 0;
    virtual AbstractType * multiply(AbstractType const * r_value) const = 0;
    virtual AbstractType * to_power(AbstractType const * r_value) const = 0;

    /***
     * @brief Functions
     */
    virtual AbstractType * sin(void) const = 0;
    virtual AbstractType * cos(void) const = 0;
    virtual AbstractType * tg(void) const = 0;
    virtual AbstractType * ctg(void) const = 0;
    virtual AbstractType * abs(void) const = 0;
    virtual AbstractType * log(void) const = 0;
    virtual AbstractType * exp(void) const = 0;

protected:
    VarTypes Type;

    AbstractType(void){}
};

/***
 * DOUBLE Type
 */

class MathTypes::TypeDouble : public AbstractType{
public:
    TypeDouble(qreal nValue){
        this->Value = nValue;
        this->Type = MATH_VAR_TYPE_DOUBLE;
    }
    TypeDouble(void){
        this->Type = MATH_VAR_TYPE_DOUBLE;
    }

    VarTypes GetType(void) const override{
        return this->Type;
    }
    qreal GetValue(void) const{
        return this->Value;
    }
    void SetValue(qreal nValue){
        this->Value = nValue;
    }

    /***
     * @brief Operators
     */
    AbstractType * assign(AbstractType const * r_value) override;
    AbstractType * sum(AbstractType const * r_value) const override;
    AbstractType * subtract(AbstractType const * r_value) const override;
    AbstractType * divide(AbstractType const * r_value) const override;
    AbstractType * multiply(AbstractType const * r_value) const override;
    AbstractType * to_power(AbstractType const * r_value) const override;

    /***
     * @brief Functions
     */

    AbstractType * sin(void) const override;
    AbstractType * cos(void) const override;
    AbstractType * tg(void) const override;
    AbstractType * ctg(void) const override;
    AbstractType * abs(void) const override;
    AbstractType * log(void) const override;
    AbstractType * exp(void) const override;

    ~TypeDouble(void) override {}

private:
    qreal Value;
};

/***
 * VECTOR Type
 */

class MathTypes::TypeVector : public AbstractType{
public:
    TypeVector(QVector<qreal> nVector){
        this->Vector = nVector;
        this->Type = MATH_VAR_TYPE_VECTOR;
    }
    TypeVector(void){
        this->Type = MATH_VAR_TYPE_VECTOR;
    }

    VarTypes GetType(void) const override {
        return this->Type;
    }

    int Size(void) const{
        return this->Vector.size();
    }
    qreal At(int index) const{
        if (index < this->Vector.size()){
            return static_cast<qreal>(this->Vector[index]);
        }
        else{
            return static_cast<qreal>(0);
        }
    }
    void PushBack(qreal value){
        this->Vector.push_back(value);
    }
    void PopBack(void){
        this->Vector.pop_back();
    }
    void PushFront(qreal value){
        this->Vector.push_front(value);
    }
    void PopFront(void){
        this->Vector.pop_front();
    }
    QVector<qreal> GetRawCopy(void) const {
        return this->Vector;
    }
    void SetValue(QVector<qreal> NewData){
        this->Vector = NewData;
    }
    /***
     * @brief Operators
     */
    AbstractType * assign(AbstractType const * r_value) override;
    AbstractType * sum(AbstractType const * r_value) const override;
    AbstractType * subtract(AbstractType const * r_value) const override;
    AbstractType * divide(AbstractType const * r_value) const override;
    AbstractType * multiply(AbstractType const * r_value) const override;
    AbstractType * to_power(AbstractType const * r_value) const override;
    /***
     * @brief Functions
     */
    AbstractType * sin(void) const override;
    AbstractType * cos(void) const override;
    AbstractType * tg(void) const override;
    AbstractType * ctg(void) const override;
    AbstractType * abs(void) const override;
    AbstractType * log(void) const override;
    AbstractType * exp(void) const override;

    ~TypeVector(void) override {}

private:
    QVector<qreal> Vector;
};

#endif // MATHTYPES_H
