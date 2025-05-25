//
// Created by chang liu on 17/05/2025.
//

#ifndef DATABASE_H
#define DATABASE_H
#include <iostream>
#include  <string>
#include "Table.h"

class Database {
private:
    std::unordered_map<std::string ,Table> tables;

public:
    Database() = default;



    void createTable(const std::string& tableName);
    void DropTable(const std::string& tableName);
    Table* GetTable(const std::string& tableName);
    std::unordered_map<std::string,Table> getTables() const;//for alter drop sentence
    void listTables() const;


    bool saveToFile(const std::string& fileName) const;
    bool loadFromFile(const std::string& fileName);

};
#endif //DATABASE_H
