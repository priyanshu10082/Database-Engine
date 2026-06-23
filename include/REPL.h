#pragma once
#include <string>
#include "Schema.h"
#include "ExecutionEngine.h"

class REPL {
private:
    Schema schema;
    ExecutionContext* context;
    int totalRows;

    void executeInsert(const std::string& args);
    void executeSelect(const std::string& args);

public:
    REPL(ExecutionContext* ctx, const Schema& schema);
    void start();
};
