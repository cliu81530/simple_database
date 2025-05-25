//
// Created by chang liu on 16/05/2025.
//
#include "Row.h"

#include "column.h"

void Row:: addValue(const Value& v ){  values.push_back(v);    }
void Row:: removeValue(int idx) {
    if (idx >= values.size()) {
        throw std::out_of_range("Index out of bounds");
    }
    else {
        values.erase(values.begin() + idx);
    }
}
void Row:: removeAllValues() { this->values.clear();  }
void Row:: updateValue(int idx,const Value& v) {
    if (idx >= values.size()) {
        // throw
        // throw std::out_of_range("Index out of bounds");
        throw std::out_of_range("Idx out of bounds");
    }
    else {
        values[idx] = v;
    }
}


Value& Row::getValue(int idx) {
    if (idx >= values.size()) {
        throw std::out_of_range("Index out of bounds");
    }
    else {
        return values[idx];
    }
}
const std::vector<Value>& Row::getValues() const {
    return values;
}
