#include "../include/REPL.h"
#include "../include/Expression.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cctype>

REPL::REPL(ExecutionContext* ctx, const Schema& sch) 
    : context(ctx), schema(sch), totalRows(0) {}

void REPL::start() {
    std::string line;
    std::cout << "Welcome to the C++ DB Engine REPL." << std::endl;
    std::cout << "Schema is: id (INT), name (VARCHAR(32))" << std::endl;
    std::cout << "Commands: " << std::endl;
    std::cout << "  INSERT <id> <name>" << std::endl;
    std::cout << "  SELECT" << std::endl;
    std::cout << "  SELECT WHERE <name>" << std::endl;
    std::cout << "  EXIT" << std::endl;

    while (true) {
        std::cout << "db> ";
        if (!std::getline(std::cin, line)) {
            break;
        }

        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string command;
        ss >> command;

        // Make command uppercase
        for (auto& c : command) c = std::toupper(static_cast<unsigned char>(c));

        if (command == "EXIT") {
            break;
        } else if (command == "INSERT") {
            std::string args;
            std::getline(ss, args);
            executeInsert(args);
        } else if (command == "SELECT") {
            std::string args;
            std::getline(ss, args);
            executeSelect(args);
        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }
}

void REPL::executeInsert(const std::string& args) {
    std::stringstream ss(args);
    int id;
    std::string name;

    if (ss >> id >> name) {
        std::vector<Row> rowsToInsert;
        rowsToInsert.push_back(Row({DBValue(id), DBValue(name)}));

        // In a real DB, we would keep track of pages properly,
        // but for this simplified REPL we'll just insert everything at page 0
        // or append it. InsertExecutor writes to storage.
        InsertExecutor insertExec(context, schema, rowsToInsert, 0);
        insertExec.init();
        Row dummyRow;
        while (insertExec.next(&dummyRow)) {
            std::cout << "Inserted 1 row." << std::endl;
            totalRows++; // Keep track of inserted rows for SeqScan
        }
    } else {
        std::cout << "Syntax error. Usage: INSERT <id> <name>" << std::endl;
    }
}

void REPL::executeSelect(const std::string& args) {
    std::stringstream ss(args);
    std::string token;
    
    // Check if it has "WHERE"
    bool hasWhere = false;
    std::string filterName;
    if (ss >> token) {
        for (auto& c : token) c = std::toupper(static_cast<unsigned char>(c));
        if (token == "WHERE") {
            if (ss >> filterName) {
                hasWhere = true;
            } else {
                std::cout << "Syntax error. Usage: SELECT WHERE <name>" << std::endl;
                return;
            }
        }
    }

    if (hasWhere) {
        auto scanForFilter = std::make_unique<SeqScanExecutor>(context, schema, 0, totalRows);
        auto predicate = std::make_unique<ComparisonExpression>(1, DBValue(filterName)); 
        FilterExecutor filterExec(context, std::move(scanForFilter), std::move(predicate), schema);
        
        filterExec.init();
        Row row;
        int count = 0;
        while (filterExec.next(&row)) {
            std::cout << "ID: " << row.getValues()[0].intValue 
                      << ", Name: " << row.getValues()[1].stringValue << std::endl;
            count++;
        }
        std::cout << "(" << count << " rows)" << std::endl;
    } else {
        SeqScanExecutor seqScanExec(context, schema, 0, totalRows);
        seqScanExec.init();
        Row row;
        int count = 0;
        while (seqScanExec.next(&row)) {
            std::cout << "ID: " << row.getValues()[0].intValue 
                      << ", Name: " << row.getValues()[1].stringValue << std::endl;
            count++;
        }
        std::cout << "(" << count << " rows)" << std::endl;
    }
}
