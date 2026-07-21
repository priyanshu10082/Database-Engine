#include <iostream>
#include <vector>
#include "include/StorageManager.h"
#include "include/Schema.h"
#include "include/Row.h"
#include "include/ExecutionEngine.h"
#include "include/BPlusTree.h"
#include "include/REPL.h"

int main() {
    std::cout << "Initializing Database Engine with B+ Tree Index..." << std::endl;

    // 1. Define our Schema (id: INT, name: VARCHAR(32))
    std::vector<Column> columns = {
        {"id", DataType::INT, 4},
        {"name", DataType::VARCHAR, 32}
    };
    Schema schema(columns);

    // Create a StorageManager
    StorageManager storage("test_db.bin");
    ExecutionContext context;
    context.storage = &storage;

    // Create and populate the B+ Tree
    BPlusTree<int, Row> index(4);

    // Start REPL
    REPL repl(&context, schema, &index);
    repl.start();

    return 0;
}
