//
// Created by chang liu on 17/05/2025.
//

#ifndef QUERYPARSER_H
#define QUERYPARSER_H


#include"Database.h"

class QueryParser {
private:
    Database& db;
public:
    QueryParser(Database& db);

    void parseQuery(const std::string& query);

    //DDL Statements
    void parseCreateTable(const std::vector<std::string>& tokens);
    void parseDropTable(const std::vector<std::string>& tokens);
    void parseAlterTable(const std::vector<std::string>& tokens);

    //DML Statements

    void parseInsert(const std::vector<std::string>& tokens);


    //DQL
    void parseSelectQuery(const std::vector<std::string>& tokens);
    void parseWhereQuery(const std::vector<std::string>& tokens);
};
#endif //QUERYPARSER_H
