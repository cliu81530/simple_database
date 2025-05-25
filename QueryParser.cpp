#include "QueryParser.h"
#include <sstream>
#include <cctype>
#include <algorithm>
#include <unordered_map>

// Helper function to tokenize a query string with better handling of parentheses and quotes
std::vector<std::string> tokenizeQuery(const std::string& query) {
    std::vector<std::string> tokens;
    std::string token;
    bool inParens = false;
    bool inQuotes = false;
    char quoteChar = '\0';

    for (size_t i = 0; i < query.length(); i++) {
        char c = query[i];

        if (!inQuotes && (c == '\'' || c == '"')) {
            inQuotes = true;
            quoteChar = c;
            token += c;
        } else if (inQuotes && c == quoteChar) {
            inQuotes = false;
            token += c;
        } else if (!inQuotes && c == '(') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back("(");
        } else if (!inQuotes && c == ')') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(")");
        } else if (!inQuotes && c == ',') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
            tokens.push_back(",");
        } else if (!inQuotes && std::isspace(c)) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c;
        }
    }

    if (!token.empty()) {
        // Remove trailing semicolon if present
        if (token.back() == ';') {
            token.pop_back();
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}

// Helper function to convert string to lowercase
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

QueryParser::QueryParser(Database &db) : db(db) {}

// Main query parsing function
void QueryParser::parseQuery(const std::string& query) {
    std::vector<std::string> tokens = tokenizeQuery(query);

    if (tokens.empty()) {
        std::cout << "Empty query" << std::endl;
        return;
    }

    std::string command = toLower(tokens[0]);

    if (command == "create" && tokens.size() > 1 && toLower(tokens[1]) == "table") {
        parseCreateTable(tokens);
    } else if (command == "drop" && tokens.size() > 1 && toLower(tokens[1]) == "table") {
        parseDropTable(tokens);
    } else if (command == "alter" && tokens.size() > 1 && toLower(tokens[1]) == "table") {
        parseAlterTable(tokens);
    } else if (command == "insert") {
        parseInsert(tokens);
    } else if (command == "select") {
        parseSelectQuery(tokens);
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }
}

void QueryParser::parseCreateTable(const std::vector<std::string> &tokens) {
    // CREATE TABLE tablename ( col1 TYPE, col2 TYPE, ... )
    if (tokens.size() < 6) {
        throw std::invalid_argument("Invalid CREATE TABLE syntax");
    }

    std::string tableName = tokens[2];
    if (db.GetTable(tableName) != nullptr) {
        throw std::invalid_argument("Table already exists");
    }

    // Find opening and closing parentheses
    size_t openParen = 0, closeParen = 0;
    for (size_t i = 3; i < tokens.size(); i++) {
        if (tokens[i] == "(") {
            openParen = i;
            break;
        }
    }

    for (size_t i = tokens.size() - 1; i > openParen; i--) {
        if (tokens[i] == ")") {
            closeParen = i;
            break;
        }
    }

    if (openParen == 0 || closeParen == 0 || closeParen <= openParen) {
        throw std::runtime_error("Invalid CREATE TABLE syntax: Missing parentheses");
    }

    db.createTable(tableName);
    std::cout << tableName << " created." << std::endl;

    // Parse column definitions between parentheses
    for (size_t i = openParen + 1; i < closeParen; ) {
        // Skip commas
        if (tokens[i] == ",") {
            i++;
            continue;
        }

        if (i + 1 >= closeParen) {
            throw std::invalid_argument("Incomplete column definition");
        }

        std::string columnName = tokens[i];
        std::string columnTypeStr = tokens[i + 1];

        ColumnType columnType;
        if (columnTypeStr == "INTEGER") {
            columnType = ColumnType::INT;
        } else if (columnTypeStr == "FLOAT") {
            columnType = ColumnType::FLOAT;
        } else if (columnTypeStr == "STRING") {
            columnType = ColumnType::STRING;
        } else if (columnTypeStr == "BOOLEAN") {
            columnType = ColumnType::BOOLEAN;
        } else {
            throw std::invalid_argument("Invalid column type: " + columnTypeStr);
        }

        Column newColumn(columnName, columnType);
        db.GetTable(tableName)->addColumn(newColumn);

        i += 2; // Move to next column or comma
    }
}

void QueryParser::parseDropTable(const std::vector<std::string> &tokens) {
    // DROP TABLE tablename
    if (tokens.size() != 3) {
        throw std::invalid_argument("Invalid DROP TABLE syntax");
    }

    std::string tableName = tokens[2];
    if (db.GetTable(tableName) == nullptr) {
        throw std::invalid_argument("Table does not exist");
    }

    db.DropTable(tableName);
    std::cout << "Table " << tableName << " dropped." << std::endl;
}

void QueryParser::parseAlterTable(const std::vector<std::string> &tokens) {
    // ALTER TABLE tablename ADD/DROP/ALTER columnname datatype
    if (tokens.size() < 5) {
        throw std::invalid_argument("Invalid ALTER TABLE syntax");
    }

    std::string tableName = tokens[2];
    Table* table = db.GetTable(tableName);
    if (table == nullptr) {
        throw std::invalid_argument("Table does not exist");
    }

    std::string operation = toLower(tokens[3]);

    if (operation == "add") {
        if (tokens.size() != 6) {
            throw std::invalid_argument("Invalid ALTER TABLE ADD syntax");
        }

        std::string colname = tokens[4];
        std::string coltype = tokens[5];

        ColumnType columnType;
        if (coltype == "INTEGER") {
            columnType = ColumnType::INT;
        } else if (coltype == "FLOAT") {
            columnType = ColumnType::FLOAT;
        } else if (coltype == "STRING") {
            columnType = ColumnType::STRING;
        } else if (coltype == "BOOLEAN") {
            columnType = ColumnType::BOOLEAN;
        } else {
            throw std::invalid_argument("Invalid column type: " + coltype);
        }

        // Check if column already exists
        for (const auto& column : table->getColumns()) {
            if (column.getName() == colname) {
                throw std::invalid_argument("Column: " + colname + " already exists");
            }
        }

        Column newColumn(colname, columnType);
        table->addColumn(newColumn);

        // Add default values to existing rows
        auto rows = table->getRows();
        for (size_t i = 0; i < rows.size(); i++) {
            Row& currentRow = const_cast<Row&>(rows[i]);
            Value defaultValue;
            switch (columnType) {
                case ColumnType::INT:
                    defaultValue = 0;
                    break;
                case ColumnType::FLOAT:
                    defaultValue = 0.0f;
                    break;
                case ColumnType::STRING:
                    defaultValue = std::string("");
                    break;
                case ColumnType::BOOLEAN:
                    defaultValue = false;
                    break;
            }
            currentRow.addValue(defaultValue);
        }

        std::cout << "Column " << colname << " added successfully." << std::endl;
    }
    else if (operation == "drop") {
        if (tokens.size() != 6 || toLower(tokens[4]) != "column") {
            throw std::invalid_argument("Invalid ALTER TABLE DROP syntax");
        }

        std::string colname = tokens[5];
        const auto& cols = table->getColumns();
        auto it = std::find_if(cols.begin(), cols.end(), [&](const Column& col) {
            return col.getName() == colname;
        });

        if (it == cols.end()) {
            throw std::invalid_argument("Column " + colname + " does not exist");
        }

        int colIndex = std::distance(cols.begin(), it);

        // Remove column from table
        std::vector<Column> newColumns;
        for (const auto& col : cols) {
            if (col.getName() != colname) {
                newColumns.push_back(col);
            }
        }

        table->clearColumn();
        for (const auto& col : newColumns) {
            table->addColumn(col);
        }

        // Remove corresponding values from all rows
        table->dropAllRow(); // This will need to be implemented properly

        std::cout << "Column " << colname << " dropped successfully." << std::endl;
    }
}

void QueryParser::parseInsert(const std::vector<std::string> &tokens) {
    // INSERT INTO table_name (column1, column2, ...) VALUES (value1, value2, ...)

    if (tokens.size() < 6) {
        throw std::invalid_argument("Invalid INSERT syntax: Not enough tokens");
    }

    if (toLower(tokens[0]) != "insert" || toLower(tokens[1]) != "into") {
        throw std::invalid_argument("Invalid INSERT syntax: Expected 'INSERT INTO'");
    }

    std::string tableName = tokens[2];
    Table* table = db.GetTable(tableName);

    if (table == nullptr) {
        throw std::invalid_argument("Table '" + tableName + "' does not exist");
    }

    // Find the column list and values list
    size_t columnStart = 0, columnEnd = 0;
    size_t valuesStart = 0, valuesEnd = 0;
    bool foundValues = false;

    for (size_t i = 3; i < tokens.size(); i++) {
        if (tokens[i] == "(" && columnStart == 0) {
            columnStart = i + 1;
        } else if (tokens[i] == ")" && columnStart > 0 && columnEnd == 0) {
            columnEnd = i;
        } else if (toLower(tokens[i]) == "values") {
            foundValues = true;
        } else if (tokens[i] == "(" && foundValues && valuesStart == 0) {
            valuesStart = i + 1;
        } else if (tokens[i] == ")" && valuesStart > 0 && valuesEnd == 0) {
            valuesEnd = i;
            break;
        }
    }

    if (columnStart == 0 || columnEnd == 0 || valuesStart == 0 || valuesEnd == 0) {
        throw std::invalid_argument("Invalid INSERT syntax: Missing parentheses or VALUES keyword");
    }

    // Parse column names
    std::vector<std::string> columnNames;
    for (size_t i = columnStart; i < columnEnd; i++) {
        if (tokens[i] != ",") {
            columnNames.push_back(tokens[i]);
        }
    }

    // Parse values
    std::vector<std::string> valueStrings;
    for (size_t i = valuesStart; i < valuesEnd; i++) {
        if (tokens[i] != ",") {
            std::string value = tokens[i];
            // Remove quotes if present
            if (value.size() >= 2 && value.front() == '\'' && value.back() == '\'') {
                value = value.substr(1, value.size() - 2);
            }
            valueStrings.push_back(value);
        }
    }

    if (columnNames.size() != valueStrings.size()) {
        throw std::invalid_argument("Column count (" + std::to_string(columnNames.size()) +
                                   ") does not match value count (" + std::to_string(valueStrings.size()) + ")");
    }

    // Create a map of column names to indices
    const auto& tableColumns = table->getColumns();
    std::unordered_map<std::string, size_t> columnIndexMap;
    for (size_t i = 0; i < tableColumns.size(); i++) {
        columnIndexMap[tableColumns[i].getName()] = i;
    }

    // Create row with default values
    std::vector<Value> rowValues(tableColumns.size());
    for (size_t i = 0; i < tableColumns.size(); i++) {
        switch (tableColumns[i].getType()) {
            case ColumnType::INT:
                rowValues[i] = 0;
                break;
            case ColumnType::FLOAT:
                rowValues[i] = 0.0f;
                break;
            case ColumnType::STRING:
                rowValues[i] = std::string("");
                break;
            case ColumnType::BOOLEAN:
                rowValues[i] = false;
                break;
        }
    }

    // Set specified values
    for (size_t i = 0; i < columnNames.size(); i++) {
        auto it = columnIndexMap.find(columnNames[i]);
        if (it == columnIndexMap.end()) {
            throw std::invalid_argument("Column '" + columnNames[i] + "' does not exist");
        }

        size_t colIndex = it->second;
        ColumnType colType = tableColumns[colIndex].getType();
        std::string valueStr = valueStrings[i];

        try {
            switch (colType) {
                case ColumnType::INT:
                    rowValues[colIndex] = std::stoi(valueStr);
                    break;
                case ColumnType::FLOAT:
                    rowValues[colIndex] = std::stof(valueStr);
                    break;
                case ColumnType::STRING:
                    rowValues[colIndex] = valueStr;
                    break;
                case ColumnType::BOOLEAN:
                    rowValues[colIndex] = (toLower(valueStr) == "true");
                    break;
            }
        } catch (const std::exception& e) {
            throw std::invalid_argument("Error converting value '" + valueStr + "': " + e.what());
        }
    }

    // Create and add row
    Row newRow;
    for (const auto& value : rowValues) {
        newRow.addValue(value);
    }

    table->addRow(newRow);
    std::cout << "1 row inserted." << std::endl;
}

void QueryParser::parseSelectQuery(const std::vector<std::string>& tokens) {
    if (tokens.size() < 4) {
        std::cout << "Invalid SELECT statement" << std::endl;
        return;
    }

    // Find FROM keyword
    size_t fromPos = 0;
    for (size_t i = 1; i < tokens.size(); i++) {
        if (toLower(tokens[i]) == "from") {
            fromPos = i;
            break;
        }
    }

    if (fromPos == 0 || fromPos >= tokens.size() - 1) {
        std::cout << "Invalid SELECT statement: missing FROM clause" << std::endl;
        return;
    }

    std::string tableName = tokens[fromPos + 1];
    Table* table = db.GetTable(tableName);
    if (!table) {
        std::cout << "Table " << tableName << " does not exist" << std::endl;
        return;
    }

    // Parse column list
    std::vector<std::string> selectedColumns;
    const auto& tableColumns = table->getColumns();

    if (tokens[1] == "*") {
        // Select all columns
        for (const auto& col : tableColumns) {
            selectedColumns.push_back(col.getName());
        }
    } else {
        // Parse specific columns
        for (size_t i = 1; i < fromPos; i++) {
            if (tokens[i] != ",") {
                selectedColumns.push_back(tokens[i]);
            }
        }
    }

    if (selectedColumns.empty()) {
        std::cout << "No columns specified" << std::endl;
        return;
    }

    // Get column indices
    std::vector<int> colIndices;
    for (const auto& colName : selectedColumns) {
        bool found = false;
        for (size_t i = 0; i < tableColumns.size(); i++) {
            if (tableColumns[i].getName() == colName) {
                colIndices.push_back(static_cast<int>(i));
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "Column \"" << colName << "\" does not exist" << std::endl;
            return;
        }
    }

    // Print header
    for (size_t i = 0; i < selectedColumns.size(); i++) {
        std::cout << selectedColumns[i];
        if (i < selectedColumns.size() - 1) {
            std::cout << " | ";
        }
    }
    std::cout << std::endl;

    // Print separator
    for (size_t i = 0; i < selectedColumns.size(); i++) {
        for (size_t j = 0; j < selectedColumns[i].size(); j++) {
            std::cout << "-";
        }
        if (i < selectedColumns.size() - 1) {
            std::cout << "-+-";
        }
    }
    std::cout << std::endl;

    // Print rows
    const auto& rows = table->getRows();
    for (const auto& row : rows) {
        const auto& values = row.getValues();
        for (size_t i = 0; i < colIndices.size(); i++) {
            int idx = colIndices[i];
            if (idx >= 0 && idx < static_cast<int>(values.size())) {
                const Value& val = values[idx];
                std::visit([](const auto& v) {
                    std::cout << v;
                }, val);
            }
            if (i < colIndices.size() - 1) {
                std::cout << " | ";
            }
        }
        std::cout << std::endl;
    }
}

void QueryParser::parseWhereQuery(const std::vector<std::string> &tokens) {
    // This function would need to be integrated with parseSelectQuery
    // for a complete WHERE clause implementation
    // For now, it's a placeholder for future enhancement
    std::cout << "WHERE clause parsing not yet implemented in this version" << std::endl;
}