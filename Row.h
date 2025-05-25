//
// Created by chang liu on 16/05/2025.
//

#ifndef ROW_H
#define ROW_H
#include <vector>

using Value = std::variant<int, float, std::string, bool>;

class Row {
private:
    std::vector<Value> values;
public:
    Row() = default;


    void addValue(const Value& v) ;
    void removeValue(int idx) ;
    void removeAllValues() ;
    void updateValue(int idx,const Value& v) ;


     Value& getValue(int idx) ;
    const std::vector<Value>& getValues() const;
};








#endif //ROW_H
