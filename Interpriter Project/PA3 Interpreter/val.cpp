/*Marco Scherillo 
RA8
NJIT 2024
*/
#include "val.h"
#include <cmath>
using namespace std;
//this reprecents val 1 
//op represents val 2
// all functions return an Value object.

Value Value::operator+(const Value& op) const{
    
    if(this->IsReal() && op.IsInt()){
        return Value(this->GetReal() + op.GetInt());
    }else if(this->IsInt() && op.IsReal()){
        return Value(this->GetInt() + op.GetReal());
    }else if(this->IsInt() && op.IsInt()){
        return Value(this->GetInt() + op.GetInt()); 
    }else if(this->IsReal() && op.IsReal()){
        return Value(this->GetReal() + op.GetReal());
    }else {
        return Value(); 
    }
}
Value Value::operator-(const Value& op) const{
    if(this->IsReal() && op.IsInt()){
        return Value(this->GetReal() - op.GetInt());
    }else if(this->IsInt() && op.IsReal()){
        return Value(this->GetInt() - op.GetReal());
    }else if(this->IsInt() && op.IsInt()){
        return Value(this->GetInt() - op.GetInt()); 
    }else if(this->IsReal() && op.IsReal()){
        return Value(this->GetReal() - op.GetReal());
    }else{
        return Value();
    }
}
Value Value::operator==(const Value& op) const{
    if (IsInt() && op.IsReal()) {
        double leftInt = static_cast<double>(GetInt());
        return Value(leftInt == op.GetReal());
    } 

    if (IsReal() && op.IsInt()) {
        double rightInt = static_cast<double>(op.GetInt());
        return Value(GetReal() == rightInt);
    }

    if (GetType() == op.GetType()) {
        if (IsInt() && op.IsInt()) {
            return Value(GetInt() == op.GetInt());
        } else if (IsReal() && op.IsReal()) {
            return Value(GetReal() == op.GetReal());
        } else if (IsString() && op.IsString()) {
            return Value(GetString() == op.GetString());
        } else if (IsBool() && op.IsBool()) {
            return Value(GetBool() == op.GetBool());
        }
    } else {
        return Value();
    }
    return Value();  // remember to remove this marco 
}

Value Value::operator/(const Value& op) const{
    return 0;
}
Value Value::operator>(const Value& op) const{
    return 0;
}

Value Value::operator*(const Value &op) const
{    
    if(this->IsReal() && op.IsInt()){
        return Value(this->GetReal() * op.GetInt());
    }else if(this->IsInt() && op.IsReal()){
        return Value(this->GetInt() * op.GetReal());
    }else if(this->IsInt() && op.IsInt()){
        return Value(this->GetInt() * op.GetInt()); 
    }else if(this->IsReal() && op.IsReal()){
        return Value(this->GetReal() * op.GetReal());
    }else{
        return Value();
    }
}

Value Value::Catenate(const Value &op) const
{   
   if(this->IsString() && op.IsString()){
       return Value(this->GetString() + op.GetString());
   }else{
    return Value();
   }
}

Value Value::Power(const Value &op) const
{   
     if(this->IsReal() && op.IsInt()){
        return Value( pow(this->GetReal(), op.GetInt()) );
    }else if(this->IsInt() && op.IsReal()){
        return Value( pow(this->GetInt(), op.GetReal()) );
    }else if(this->IsInt() && op.IsInt()){
        return Value( pow(this->GetInt(), op.GetInt()) ); 
    }else if(this->IsReal() && op.IsReal()){
        return Value( pow(this->GetReal(), op.GetReal()));
    }else{ 
        return Value();
    }
}

Value Value::operator<(const Value &op) const
{
    if(this->IsReal() && op.IsInt()){
        return Value(this->GetReal()<op.GetInt()); 
    }else if(this->IsInt() && op.IsInt()){
        return Value(this->GetInt() < op.GetInt());
    }else if(this->IsReal() && op.IsReal()){
        return Value(this->GetReal() < op.GetReal());
    }else if(this->IsInt() && op.IsReal()){
        return Value(this->GetInt() < op.GetReal()); 
    }else{
        return Value();
    }
}

