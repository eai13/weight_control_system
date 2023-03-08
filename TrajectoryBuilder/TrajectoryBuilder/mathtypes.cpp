#include "mathtypes.h"

/***
 * Math types and functions overload
 */

/***
 * DOUBLE Type
 */

MathTypes::AbstractType * MathTypes::TypeDouble::assign(AbstractType const * r_value) {
    if (r_value->GetType() == MATH_VAR_TYPE_DOUBLE){
        TypeDouble const * r_v = dynamic_cast<TypeDouble const *>(r_value);
        this->Value = r_v->GetValue();
        return this;
    }
    else{
        return nullptr;
    }
}

MathTypes::AbstractType * MathTypes::TypeDouble::sum(AbstractType const * r_value) const {
    switch(r_value->GetType()){
    case(MATH_VAR_TYPE_DOUBLE):{
        TypeDouble * ret_val = new TypeDouble();
        TypeDouble const * r_v = dynamic_cast<TypeDouble const *>(r_value);
        ret_val->SetValue(this->Value + r_v->GetValue());
        return ret_val;
    }
    case(MATH_VAR_TYPE_VECTOR):{
        TypeVector const * r_v = dynamic_cast<TypeVector const *>(r_value);
        QVector<qreal> ret_raw_vec = r_v->GetRawCopy();
        for (auto iter = ret_raw_vec.begin(); iter != ret_raw_vec.end(); iter++)
            (*iter) = this->Value + (*iter);
        TypeVector * ret_vec = new TypeVector(ret_raw_vec);
        return ret_vec;
    }
    default:{
        return nullptr;
    }
    }
}

MathTypes::AbstractType * MathTypes::TypeDouble::subtract(AbstractType const * r_value) const {
    switch(r_value->GetType()){
    case(MATH_VAR_TYPE_DOUBLE):{
        TypeDouble * ret_val = new TypeDouble();
        TypeDouble const * r_v = dynamic_cast<TypeDouble const *>(r_value);
        ret_val->SetValue(this->Value - r_v->GetValue());
        return ret_val;
        break;
    }
    case(MATH_VAR_TYPE_VECTOR):{
        TypeVector const * r_v = dynamic_cast<TypeVector const *>(r_value);
        QVector<qreal> ret_raw_vec = r_v->GetRawCopy();
        for (auto iter = ret_raw_vec.begin(); iter != ret_raw_vec.end(); iter++)
            (*iter) = this->Value - (*iter);
        TypeVector * ret_vec = new TypeVector(ret_raw_vec);
        return ret_vec;
        break;
    }
    default:{
        return nullptr;
    }
    }
}
MathTypes::AbstractType * MathTypes::TypeDouble::divide(AbstractType const * r_value) const {
    switch(r_value->GetType()){
    case(MATH_VAR_TYPE_DOUBLE):{
        TypeDouble * ret_val = new TypeDouble();
        TypeDouble const * r_v = dynamic_cast<TypeDouble const *>(r_value);
        ret_val->SetValue(this->Value / r_v->GetValue());
        return ret_val;
        break;
    }
    case(MATH_VAR_TYPE_VECTOR):{
        TypeVector const * r_v = dynamic_cast<TypeVector const *>(r_value);
        QVector<qreal> ret_raw_vec = r_v->GetRawCopy();
        for (auto iter = ret_raw_vec.begin(); iter != ret_raw_vec.end(); iter++)
            (*iter) = this->Value / (*iter);
        TypeVector * ret_vec = new TypeVector(ret_raw_vec);
        return ret_vec;
        break;
    }
    default:{
        return nullptr;
    }
    }
}
MathTypes::AbstractType * MathTypes::TypeDouble::multiply(AbstractType const * r_value) const {
    switch(r_value->GetType()){
    case(MATH_VAR_TYPE_DOUBLE):{
        TypeDouble * ret_val = new TypeDouble();
        TypeDouble const * r_v = dynamic_cast<TypeDouble const *>(r_value);
        ret_val->SetValue(this->Value * r_v->GetValue());
        return ret_val;
        break;
    }
    case(MATH_VAR_TYPE_VECTOR):{
        TypeVector const * r_v = dynamic_cast<TypeVector const *>(r_value);
        QVector<qreal> ret_raw_vec = r_v->GetRawCopy();
        for (auto iter = ret_raw_vec.begin(); iter != ret_raw_vec.end(); iter++)
            (*iter) = this->Value * (*iter);
        TypeVector * ret_vec = new TypeVector(ret_raw_vec);
        return ret_vec;
        break;
    }
    default:{
        return nullptr;
    }
    }
}
MathTypes::AbstractType * MathTypes::TypeDouble::to_power(AbstractType const * r_value) const {
    switch(r_value->GetType()){
    case(MATH_VAR_TYPE_DOUBLE):{
        TypeDouble * ret_val = new TypeDouble();
        TypeDouble const * r_v = dynamic_cast<TypeDouble const *>(r_value);
        ret_val->SetValue(qPow(this->Value, r_v->GetValue()));
        return ret_val;
        break;
    }
    case(MATH_VAR_TYPE_VECTOR):{
        TypeVector const * r_v = dynamic_cast<TypeVector const *>(r_value);
        QVector<qreal> ret_raw_vec = r_v->GetRawCopy();
        for (auto iter = ret_raw_vec.begin(); iter != ret_raw_vec.end(); iter++)
            (*iter) = qPow(this->Value, (*iter));
        TypeVector * ret_vec = new TypeVector(ret_raw_vec);
        return ret_vec;
        break;
    }
    default:{
        return nullptr;
    }
    }
}

MathTypes::AbstractType * MathTypes::TypeDouble::sin(void) const {
    TypeDouble * ret_val = new TypeDouble(std::sin(this->Value));
    return ret_val;
}
MathTypes::AbstractType * MathTypes::TypeDouble::cos(void) const {
    TypeDouble * ret_val = new TypeDouble(std::cos(this->Value));
    return ret_val;
}
MathTypes::AbstractType * MathTypes::TypeDouble::tg(void) const {
    TypeDouble * ret_val = new TypeDouble(std::tan(this->Value));
    return ret_val;
}
MathTypes::AbstractType * MathTypes::TypeDouble::ctg(void) const {
    TypeDouble * ret_val = new TypeDouble(1 / std::tan(this->Value));
    return ret_val;
}
MathTypes::AbstractType * MathTypes::TypeDouble::abs(void) const {
    TypeDouble * ret_val = new TypeDouble(std::abs(this->Value));
    return ret_val;
}
MathTypes::AbstractType * MathTypes::TypeDouble::log(void) const {
    TypeDouble * ret_val = new TypeDouble(std::log(this->Value));
    return ret_val;
}
MathTypes::AbstractType * MathTypes::TypeDouble::exp(void) const {
    TypeDouble * ret_val = new TypeDouble(std::exp(this->Value));
    return ret_val;
}

/***
 * VECTOR OF DOUBLES Type
 */

MathTypes::AbstractType * MathTypes::TypeVector::assign(AbstractType const * r_value) {
    if (r_value->GetType() == MATH_VAR_TYPE_VECTOR){
        TypeVector const * r_v = dynamic_cast<TypeVector const *>(r_value);
        TypeVector * ret_vec = new TypeVector(r_v->GetRawCopy());
        this->Vector = r_v->GetRawCopy();
        return ret_vec;
    }
    else{
        return nullptr;
    }
}

MathTypes::AbstractType * MathTypes::TypeVector::sum(AbstractType const * r_value) const {
    switch(r_value->GetType()){
    case(MATH_VAR_TYPE_DOUBLE):{
        TypeDouble const * r_v = dynamic_cast<TypeDouble const *>(r_value);
        QVector<qreal> ret_raw_vec = this->GetRawCopy();
        for (auto iter = ret_raw_vec.begin(); iter != ret_raw_vec.end(); iter++)
            (*iter) = (*iter) + r_v->GetValue();
        TypeVector * ret_vec = new TypeVector(ret_raw_vec);
        return ret_vec;
    }
    case(MATH_VAR_TYPE_VECTOR):{
        TypeVector const * r_v = dynamic_cast<TypeVector const *>(r_value);
        if (this->Vector.size() != r_v->Size()){
            return nullptr;
        }
        QVector<qreal> l_value_vec = this->GetRawCopy();
        QVector<qreal> r_value_vec = r_v->GetRawCopy();
        auto l_value_iter = l_value_vec.begin();
        auto r_value_iter = r_value_vec.begin();
        for (; (l_value_iter != l_value_vec.end()) && (r_value_iter != r_value_vec.end()); l_value_iter++, r_value_iter++)
            (*l_value_iter) += (*r_value_iter);
        TypeVector * ret_vec = new TypeVector(l_value_vec);
        return ret_vec;
    }
    default:{
        return nullptr;
    }
    }
}
MathTypes::AbstractType * MathTypes::TypeVector::subtract(AbstractType const * r_value) const {
    switch(r_value->GetType()){
    case(MATH_VAR_TYPE_DOUBLE):{
        TypeDouble const * r_v = dynamic_cast<TypeDouble const *>(r_value);
        QVector<qreal> ret_raw_vec = this->GetRawCopy();
        for (auto iter = ret_raw_vec.begin(); iter != ret_raw_vec.end(); iter++)
            (*iter) = (*iter) - r_v->GetValue();
        TypeVector * ret_vec = new TypeVector(ret_raw_vec);
        return ret_vec;
    }
    case(MATH_VAR_TYPE_VECTOR):{
        TypeVector const * r_v = dynamic_cast<TypeVector const *>(r_value);
        if (this->Vector.size() != r_v->Size()){
            return nullptr;
        }
        QVector<qreal> l_value_vec = this->GetRawCopy();
        QVector<qreal> r_value_vec = r_v->GetRawCopy();
        auto l_value_iter = l_value_vec.begin();
        auto r_value_iter = r_value_vec.begin();
        for (; (l_value_iter != l_value_vec.end()) && (r_value_iter != r_value_vec.end()); l_value_iter++, r_value_iter++)
            (*l_value_iter) -= (*r_value_iter);
        TypeVector * ret_vec = new TypeVector(l_value_vec);
        return ret_vec;
    }
    default:{
        return nullptr;
    }
    }
}
MathTypes::AbstractType * MathTypes::TypeVector::divide(AbstractType const * r_value) const {
    switch(r_value->GetType()){
    case(MATH_VAR_TYPE_DOUBLE):{
        TypeDouble const * r_v = dynamic_cast<TypeDouble const *>(r_value);
        QVector<qreal> ret_raw_vec = this->GetRawCopy();
        for (auto iter = ret_raw_vec.begin(); iter != ret_raw_vec.end(); iter++)
            (*iter) = (*iter) / r_v->GetValue();
        TypeVector * ret_vec = new TypeVector(ret_raw_vec);
        return ret_vec;
    }
    case(MATH_VAR_TYPE_VECTOR):{
        TypeVector const * r_v = dynamic_cast<TypeVector const *>(r_value);
        if (this->Vector.size() != r_v->Size()){
            return nullptr;
        }
        QVector<qreal> l_value_vec = this->GetRawCopy();
        QVector<qreal> r_value_vec = r_v->GetRawCopy();
        auto l_value_iter = l_value_vec.begin();
        auto r_value_iter = r_value_vec.begin();
        for (; (l_value_iter != l_value_vec.end()) && (r_value_iter != r_value_vec.end()); l_value_iter++, r_value_iter++)
            (*l_value_iter) /= (*r_value_iter);
        TypeVector * ret_vec = new TypeVector(l_value_vec);
        return ret_vec;
    }
    default:{
        return nullptr;
    }
    }
}
MathTypes::AbstractType * MathTypes::TypeVector::multiply(AbstractType const * r_value) const {
    switch(r_value->GetType()){
    case(MATH_VAR_TYPE_DOUBLE):{
        TypeDouble const * r_v = dynamic_cast<TypeDouble const *>(r_value);
        QVector<qreal> ret_raw_vec = this->GetRawCopy();
        for (auto iter = ret_raw_vec.begin(); iter != ret_raw_vec.end(); iter++)
            (*iter) = (*iter) * r_v->GetValue();
        TypeVector * ret_vec = new TypeVector(ret_raw_vec);
        return ret_vec;
    }
    case(MATH_VAR_TYPE_VECTOR):{
        TypeVector const * r_v = dynamic_cast<TypeVector const *>(r_value);
        if (this->Vector.size() != r_v->Size()){
            return nullptr;
        }
        QVector<qreal> l_value_vec = this->GetRawCopy();
        QVector<qreal> r_value_vec = r_v->GetRawCopy();
        auto l_value_iter = l_value_vec.begin();
        auto r_value_iter = r_value_vec.begin();
        for (; (l_value_iter != l_value_vec.end()) && (r_value_iter != r_value_vec.end()); l_value_iter++, r_value_iter++)
            (*l_value_iter) *= (*r_value_iter);
        TypeVector * ret_vec = new TypeVector(l_value_vec);
        return ret_vec;
    }
    default:{
        return nullptr;
    }
    }
}
MathTypes::AbstractType * MathTypes::TypeVector::to_power(AbstractType const * r_value) const {
    switch(r_value->GetType()){
    case(MATH_VAR_TYPE_DOUBLE):{
        TypeDouble const * r_v = dynamic_cast<TypeDouble const *>(r_value);
        QVector<qreal> ret_raw_vec = this->GetRawCopy();
        for (auto iter = ret_raw_vec.begin(); iter != ret_raw_vec.end(); iter++)
            (*iter) = qPow((*iter), r_v->GetValue());
        TypeVector * ret_vec = new TypeVector(ret_raw_vec);
        return ret_vec;
    }
    case(MATH_VAR_TYPE_VECTOR):{
        TypeVector const * r_v = dynamic_cast<TypeVector const *>(r_value);
        if (this->Vector.size() != r_v->Size()){
            return nullptr;
        }
        QVector<qreal> l_value_vec = this->GetRawCopy();
        QVector<qreal> r_value_vec = r_v->GetRawCopy();
        auto l_value_iter = l_value_vec.begin();
        auto r_value_iter = r_value_vec.begin();
        for (; (l_value_iter != l_value_vec.end()) && (r_value_iter != r_value_vec.end()); l_value_iter++, r_value_iter++)
            (*l_value_iter) = qPow((*l_value_iter), (*r_value_iter));
        TypeVector * ret_vec = new TypeVector(l_value_vec);
        return ret_vec;
    }
    case(MATH_VAR_TYPE_NONE):{
        return nullptr;
    }
    }
}

MathTypes::AbstractType * MathTypes::TypeVector::sin(void) const {
    QVector<qreal> raw_vec = this->GetRawCopy();
    for (auto iter = raw_vec.begin(); iter != raw_vec.end(); iter++)
        (*iter) = std::sin(*iter);
    return new TypeVector(raw_vec);
}
MathTypes::AbstractType * MathTypes::TypeVector::cos(void) const {
    QVector<qreal> raw_vec = this->GetRawCopy();
    for (auto iter = raw_vec.begin(); iter != raw_vec.end(); iter++)
        (*iter) = std::cos(*iter);
    return new TypeVector(raw_vec);
}
MathTypes::AbstractType * MathTypes::TypeVector::tg(void) const {
    QVector<qreal> raw_vec = this->GetRawCopy();
    for (auto iter = raw_vec.begin(); iter != raw_vec.end(); iter++)
        (*iter) = std::tan(*iter);
    return new TypeVector(raw_vec);
}
MathTypes::AbstractType * MathTypes::TypeVector::ctg(void) const {
    QVector<qreal> raw_vec = this->GetRawCopy();
    for (auto iter = raw_vec.begin(); iter != raw_vec.end(); iter++)
        (*iter) = (1 / std::tan(*iter));
    return new TypeVector(raw_vec);
}
MathTypes::AbstractType * MathTypes::TypeVector::abs(void) const {
    QVector<qreal> raw_vec = this->GetRawCopy();
    for (auto iter = raw_vec.begin(); iter != raw_vec.end(); iter++)
        (*iter) = std::abs(*iter);
    return new TypeVector(raw_vec);
}
MathTypes::AbstractType * MathTypes::TypeVector::log(void) const {
    QVector<qreal> raw_vec = this->GetRawCopy();
    for (auto iter = raw_vec.begin(); iter != raw_vec.end(); iter++)
        (*iter) = std::log(*iter);
    return new TypeVector(raw_vec);
}
MathTypes::AbstractType * MathTypes::TypeVector::exp(void) const {
    QVector<qreal> raw_vec = this->GetRawCopy();
    for (auto iter = raw_vec.begin(); iter != raw_vec.end(); iter++)
        (*iter) = std::exp(*iter);
    return new TypeVector(raw_vec);
}
