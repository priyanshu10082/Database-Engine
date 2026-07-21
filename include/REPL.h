#pragma once
#include <string>
#include "Schema.h"
#include "ExecutionEngine.h"
#include "BPlusTree.h"

class REPL {
private:
    Schema schema;
    ExecutionContext* context;
    int totalRows;
    BPlusTree<int, Row>* index;

    void executeInsert(const std::string& args);
    void executeSelect(const std::string& args);

public:
    REPL(ExecutionContext* ctx, const Schema& schema, BPlusTree<int, Row>* idx);
    void start();
};
