#include <iostream>
#include <string>
#include <stdexcept>
#include "Database.h"
#include "QueryParser.h"

void printMenu() {
    std::cout << "\n=== SQL Database Management System ===" << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "1. CREATE TABLE tablename (col1 TYPE, col2 TYPE, ...)" << std::endl;
    std::cout << "2. DROP TABLE tablename" << std::endl;
    std::cout << "3. ALTER TABLE tablename ADD columnname TYPE" << std::endl;
    std::cout << "4. ALTER TABLE tablename DROP COLUMN columnname" << std::endl;
    std::cout << "5. ALTER TABLE tablename ALTER COLUMN columnname TYPE" << std::endl;
    std::cout << "6. INSERT INTO tablename (col1, col2, ...) VALUES (val1, val2, ...)" << std::endl;
    std::cout << "7. SELECT * FROM tablename" << std::endl;
    std::cout << "8. SELECT col1, col2 FROM tablename" << std::endl;
    std::cout << "9. list - Show all tables" << std::endl;
    std::cout << "10. demo - Run demonstration queries" << std::endl;
    std::cout << "11. save filename - Save database to file" << std::endl;
    std::cout << "12. load filename - Load database from file" << std::endl;
    std::cout << "13. help - Show this menu" << std::endl;
    std::cout << "14. exit - Exit the program" << std::endl;
    std::cout << "\nSupported types: INTEGER, FLOAT, STRING, BOOLEAN" << std::endl;
    std::cout << "=====================================" << std::endl;
}

void runDemo(Database& db, QueryParser& parser) {
    std::cout << "\n=== Running Demonstration ===" << std::endl;

    try {
        // Create a users table
        std::cout << "\n1. Creating 'users' table..." << std::endl;
        std::string createQuery = "CREATE TABLE users ( id INTEGER, name STRING, age INTEGER, active BOOLEAN )";
        parser.parseQuery(createQuery);
        std::cout << "✓ Table 'users' created successfully!" << std::endl;

        // Insert some sample data
        std::cout << "\n2. Inserting sample data..." << std::endl;

        std::string insert1 = "INSERT INTO users ( id, name, age, active ) VALUES ( 1, 'John Doe', 25, true )";
        parser.parseQuery(insert1);
        std::cout << "✓ Inserted John Doe" << std::endl;

        std::string insert2 = "INSERT INTO users ( id, name, age, active ) VALUES ( 2, 'Jane Smith', 30, true )";
        parser.parseQuery(insert2);
        std::cout << "✓ Inserted Jane Smith" << std::endl;

        std::string insert3 = "INSERT INTO users ( id, name, age, active ) VALUES ( 3, 'Bob Johnson', 22, false )";
        parser.parseQuery(insert3);
        std::cout << "✓ Inserted Bob Johnson" << std::endl;

        // Select all data
        std::cout << "\n3. Selecting all users:" << std::endl;
        std::string selectAll = "SELECT * FROM users";
        parser.parseQuery(selectAll);

        // Select specific columns
        std::cout << "\n4. Selecting specific columns (name, age):" << std::endl;
        std::string selectSpecific = "SELECT name, age FROM users";
        parser.parseQuery(selectSpecific);

        // Create another table
        std::cout << "\n5. Creating 'products' table..." << std::endl;
        std::string createProducts = "CREATE TABLE products ( id INTEGER, name STRING, price FLOAT, in_stock BOOLEAN )";
        parser.parseQuery(createProducts);
        std::cout << "✓ Table 'products' created successfully!" << std::endl;

        // Insert products
        std::cout << "\n6. Inserting product data..." << std::endl;
        std::string productInsert1 = "INSERT INTO products ( id, name, price, in_stock ) VALUES ( 1, 'Laptop', 999.99, true )";
        parser.parseQuery(productInsert1);

        std::string productInsert2 = "INSERT INTO products ( id, name, price, in_stock ) VALUES ( 2, 'Mouse', 25.50, true )";
        parser.parseQuery(productInsert2);

        std::string productInsert3 = "INSERT INTO products ( id, name, price, in_stock ) VALUES ( 3, 'Keyboard', 75.00, false )";
        parser.parseQuery(productInsert3);
        std::cout << "✓ Product data inserted!" << std::endl;

        // Show products
        std::cout << "\n7. All products:" << std::endl;
        std::string selectProducts = "SELECT * FROM products";
        parser.parseQuery(selectProducts);

        // Alter table example
        std::cout << "\n8. Adding column to users table..." << std::endl;
        std::string alterAdd = "ALTER TABLE users ADD email STRING";
        parser.parseQuery(alterAdd);
        std::cout << "✓ Added email column!" << std::endl;

        // Show updated table structure by selecting all
        std::cout << "\n9. Users table after adding email column:" << std::endl;
        std::string selectAfterAlter = "SELECT * FROM users";
        parser.parseQuery(selectAfterAlter);

        std::cout << "\n=== Demo completed successfully! ===" << std::endl;

    } catch (const std::exception& e) {
        std::cout << "Demo error: " << e.what() << std::endl;
    }
}

int main() {
    Database db;
    QueryParser parser(db);
    std::string input;

    std::cout << "Welcome to the SQL Database Management System!" << std::endl;
    printMenu();

    while (true) {
        std::cout << "\ndb> ";
        std::getline(std::cin, input);

        // Handle special commands
        if (input == "exit" || input == "quit") {
            std::cout << "Goodbye!" << std::endl;
            break;
        }

        if (input == "help") {
            printMenu();
            continue;
        }

        if (input == "list") {
            std::cout << "\nListing all tables:" << std::endl;
            db.listTables();
            continue;
        }

        if (input == "demo") {
            runDemo(db, parser);
            continue;
        }

        // Handle save command
        if (input.substr(0, 4) == "save") {
            if (input.length() > 5) {
                std::string filename = input.substr(5);
                if (db.saveToFile(filename)) {
                    std::cout << "Database saved to " << filename << std::endl;
                } else {
                    std::cout << "Failed to save database to " << filename << std::endl;
                }
            } else {
                std::cout << "Usage: save filename" << std::endl;
            }
            continue;
        }

        // Handle load command
        if (input.substr(0, 4) == "load") {
            if (input.length() > 5) {
                std::string filename = input.substr(5);
                if (db.loadFromFile(filename)) {
                    std::cout << "Database loaded from " << filename << std::endl;
                } else {
                    std::cout << "Failed to load database from " << filename << std::endl;
                }
            } else {
                std::cout << "Usage: load filename" << std::endl;
            }
            continue;
        }

        // Skip empty input
        if (input.empty()) {
            continue;
        }

        // Parse and execute SQL commands
        try {
            parser.parseQuery(input);
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}