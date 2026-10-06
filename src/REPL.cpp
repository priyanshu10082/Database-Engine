#include "../include/REPL.h"
#include "../include/Expression.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cctype>

REPL::REPL(ExecutionContext* ctx, const Schema& sch, BPlusTree<int, Row>* idx) 
    : context(ctx), schema(sch), index(idx) {
    totalRows = context->storage->readMetadata();  // Reading from disk, not 0
}

void REPL::start() {
    std::string line;
    std::cout << "Welcome to the C++ DB Engine REPL." << std::endl;
    std::cout << "Schema is: id (INT), name (VARCHAR(32))" << std::endl;
    std::cout << "Commands: " << std::endl;
    std::cout << "  INSERT <id> <name>" << std::endl;
    std::cout << "  SELECT" << std::endl;
    std::cout << "  SELECT WHERE id = <id>" << std::endl;
    std::cout << "  SELECT WHERE <name>" << std::endl;
    std::cout << "  EXIT" << std::endl;

    while (true) {
        std::cout << "db> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string command;
        ss >> command;
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

    if (!(ss >> id)) {
        std::cout << "Syntax error. Usage: INSERT <id> <name>" << std::endl;
        return;
    }

    std::string name;
    std::getline(ss, name);
    if (!name.empty() && name[0] == ' ') name = name.substr(1);
    if (name.empty()) {
        std::cout << "Syntax error. Usage: INSERT <id> <name>" << std::endl;
        return;
    }

    uint32_t rowSize = schema.getRowSize();
    int rowsPerPage = static_cast<int>(PAGE_SIZE / rowSize);
    int targetPageId = (totalRows / rowsPerPage) + 1;  // +1 because page 0 is metadata
    int rowInPage    = totalRows % rowsPerPage;

    Page page;
    if (rowInPage != 0) {
        context->storage->readPage(targetPageId, page);
    }

    uint32_t offset = rowInPage * rowSize;
    Row newRow({DBValue(id), DBValue(name)});
    newRow.serialize(page.data.data() + offset, schema);

    context->storage->writePage(targetPageId, page);

    index->insert(id, newRow);

    totalRows++;
    context->storage->writeMetadata(totalRows);        // persist totalRows
    std::cout << "Inserted 1 row." << std::endl;
}

void REPL::executeSelect(const std::string& args) {
    std::stringstream ss(args);
    std::string token;

    bool hasWhere = false;
    bool isIndexScan = false;
    std::string filterName;
    int filterID = -1;

    if (ss >> token) {
        for (auto& c : token) c = std::toupper(static_cast<unsigned char>(c));
        if (token == "WHERE") {
            std::string field;
            if (ss >> field) {
                std::string fieldOriginal = field;
                std::string fieldUpper = field;
                for (auto& c : fieldUpper) c = std::toupper(static_cast<unsigned char>(c));

                if (fieldUpper == "ID") {
                    char eq;
                    if (ss >> eq >> filterID && eq == '=') {
                        isIndexScan = true;
                        hasWhere = true;
                    } else {
                        std::cout << "Syntax error. Usage: SELECT WHERE id = <id>" << std::endl;
                        return;
                    }
                } else {
                    std::string rest;
                    std::getline(ss, rest);
                    filterName = fieldOriginal;
                    if (!rest.empty() && rest[0] == ' ') rest = rest.substr(1);
                    if (!rest.empty()) filterName += " " + rest;
                    hasWhere = true;
                }
            } else {
                std::cout << "Syntax error. Usage: SELECT WHERE <name>" << std::endl;
                return;
            }
        }
    }

    if (isIndexScan) {
        IndexScanExecutor idxScan(context, index, filterID);
        idxScan.init();
        Row row;
        int count = 0;
        if (idxScan.next(&row)) {
            std::cout << "ID: " << row.getValues()[0].intValue
                      << ", Name: " << row.getValues()[1].stringValue << std::endl;
            count++;
        }
        std::cout << "(" << count << " rows)" << std::endl;
    } else if (hasWhere) {
        auto scanForFilter = std::make_unique<SeqScanExecutor>(context, schema, 1, totalRows);
        // startPageId is 1 not 0 
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
        SeqScanExecutor seqScanExec(context, schema, 1, totalRows);
        // startPageId is 1 not 0
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