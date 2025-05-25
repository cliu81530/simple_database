//
// Created by chang liu on 16/05/2025.
//
#include"Table.h"
Table::Table(const std::string& n)  { this->name = n; }
Table::~Table() =default;


void Table::addColumn(const Column& c) {columns.push_back(c);};
void Table::addRow(const Row& r) {rows.push_back(r);};
void Table::dropColumn(const Column& column) {
  auto it = std::find_if(columns.begin(), columns.end(),
                       [&column](const Column& c) {
                           return c.getName() == column.getName();
                       });

  if (it != columns.end()) {
    int idx = std::distance(columns.begin(), it);
    columns.erase(it);

    // Update all rows to remove values at this column index
    for (auto& row : rows) {
      //check if idx exists
      if (idx < rows.size()) {
        row.removeValue(idx);
      }
      // row.removeValue(idx);
    }
  }
}
void Table::dropRow(int idx) {
  if(rows.size() <= idx) {
    throw std::out_of_range("Index out of bounds");
  }
  else{
    rows.erase(rows.begin() + idx);
  }
}
void Table::dropAllRow() {
  this->rows.clear();
}
void Table::clearColumn() {
  columns.clear();
}
void Table::updateRow(int idx,const Row& newRow) {
  if (idx <= rows.size() && idx >= 0) {
    if (newRow.getValues().size() == columns.size()) {
      rows[idx] = newRow;
    }
    else {
      std::cerr << "New row doesn't match table schema" << std::endl;
    }
  }
  else{std::cerr << "Invalid row index" << std::endl;}
}


  void Table::print() const {
    // Print table name
    std::cout << "Table: " << name << "\t" << std::endl;

    // Print column headers
    for (const auto& col : columns) {
      std::cout << col.getName() << "\t";
    }
    std::cout << std::endl;

    // Print separator line
    for (size_t i = 0; i < columns.size(); i++) {
      std::cout << "--------\t";
    }
    std::cout << std::endl;

    // Print rows
    for (const auto& row : rows) {
      const auto& values = row.getValues();

      for (size_t i = 0; i < values.size() && i < columns.size(); i++) {
        std::visit([](const auto& value) {
            std::cout << value << "\t";
        }, values[i]);
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
  }


std::vector<Column> Table::getColumns() const {
  return columns;
}

void Table::setColumns(const std::vector<Column>& newCols) {
  this->columns = newCols;
}

std::vector<Row> Table::getRows() const {
  return rows;
}