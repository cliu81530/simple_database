//
// Created by chang liu on 16/05/2025.
//

#ifndef COLUMN_H
#define COLUMN_H
#include <iostream>
#include <variant>



enum class ColumnType {
    INT,
    FLOAT,
    STRING,
    BOOLEAN
};
using Value = std::variant<int, float, std::string, bool>;
class Column {
    private:
      std::string name;
      ColumnType type;
      bool isNull;
    public:
      Column(const std::string& n, ColumnType t, bool isNull = true) : name(n), type(t), isNull(isNull) {}

      const std::string& getName() const;
      ColumnType getType() const;


};



#endif //COLUMN_H
