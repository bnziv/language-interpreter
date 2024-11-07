#include "val.h"

Value Value::operator+(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp + op.Itemp);
    } else if (IsInt() && op.IsReal()) {
        return Value(Itemp + op.Rtemp);
    } else if (IsReal() && op.IsInt()) {
        return Value(Rtemp + op.Itemp);
    } else if (IsReal() && op.IsReal()) {
        return Value(Rtemp + op.Rtemp);
    } else {
        return Value();
    }
}

Value Value::operator-(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp - op.Itemp);
    } else if (IsInt() && op.IsReal()) {
        return Value(Itemp - op.Rtemp);
    } else if (IsReal() && op.IsInt()) {
        return Value(Rtemp - op.Itemp);
    } else if (IsReal() && op.IsReal()) {
        return Value(Rtemp - op.Rtemp);
    } else {
        return Value();
    }
}

Value Value::operator*(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp * op.Itemp);
    } else if (IsInt() && op.IsReal()) {
        return Value(Itemp * op.Rtemp);
    } else if (IsReal() && op.IsInt()) {
        return Value(Rtemp * op.Itemp);
    } else if (IsReal() && op.IsReal()) {
        return Value(Rtemp * op.Rtemp);
    } else {
        return Value();
    }
}

Value Value::operator/(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp / op.Itemp);
    } else if (IsInt() && op.IsReal()) {
        return Value(Itemp / op.Rtemp);
    } else if (IsReal() && op.IsInt()) {
        return Value(Rtemp / op.Itemp);
    } else if (IsReal() && op.IsReal()) {
        return Value(Rtemp / op.Rtemp);
    } else {
        return Value();
    }
}



Value Value::Catenate(const Value& op) const {
    if (IsString() && op.IsString()) {
        Value ret = Value(Stemp + op.Stemp);
        return ret;
    } else {
        return Value();
    }
}

Value Value::Power(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(pow(Itemp, op.Itemp));
    } else if (IsInt() && op.IsReal()) {
        return Value(pow(Itemp, op.Rtemp));
    } else if (IsReal() && op.IsInt()) {
        return Value(pow(Rtemp, op.Itemp));
    } else if (IsReal() && op.IsReal()) {
        return Value(pow(Rtemp, op.Rtemp));
    } else {
        return Value();
    }
}

Value Value::operator==(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp == op.Itemp);
    } else if (IsInt() && op.IsReal()) {
        return Value(Itemp == op.Rtemp);
    } else if (IsReal() && op.IsInt()) {
        return Value(Rtemp == op.Itemp);
    } else if (IsReal() && op.IsReal()) {
        return Value(Rtemp == op.Rtemp);
    } else if (IsString() && op.IsString()) {
        return Value(Stemp == op.Stemp);
    } else {
        return Value();
    }
}

Value Value::operator>(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp > op.Itemp);
    } else if (IsInt() && op.IsReal()) {
        return Value(Itemp > op.Rtemp);
    } else if (IsReal() && op.IsInt()) {
        return Value(Rtemp > op.Itemp);
    } else if (IsReal() && op.IsReal()) {
        return Value(Rtemp > op.Rtemp);
    } else {
        return Value();
    }
}

Value Value::operator<(const Value& op) const {
    if (IsInt() && op.IsInt()) {
        return Value(Itemp < op.Itemp);
    } else if (IsInt() && op.IsReal()) {
        return Value(Itemp < op.Rtemp);
    } else if (IsReal() && op.IsInt()) {
        return Value(Rtemp < op.Itemp);
    } else if (IsReal() && op.IsReal()) {
        return Value(Rtemp < op.Rtemp);
    } else {
        return Value();
    }
}