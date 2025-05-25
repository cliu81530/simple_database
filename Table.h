
#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "Column.h"
#include "Row.h"

class Table {
private:
    std::string name;
    std::vector<Column> columns;
    std::vector<Row> rows;

public:
    Table(const std::string& name);
    ~Table();

    void addColumn(const Column& column);
    void addRow(const Row& row);
    void dropColumn(const Column& column);
    void dropRow(int idx);
    void dropAllRow();
    void clearColumn();
    void updateRow(int idx, const Row& row);

    void print() const;

    std::string getName() const;
    std::vector<Column> getColumns() const;
    void setColumns(const std::vector<Column>& columns);
    std::vector<Row> getRows() const;
};

#endif //TABLE_H