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
    
    Row r1({DBValue(1), DBValue("Alice")});
    Row r2({DBValue(2), DBValue("Bob")});
    Row r3({DBValue(3), DBValue("Charlie")});
    
    index.insert(1, r1);
    index.insert(2, r2);
    index.insert(3, r3);
    std::cout << "Populated B+ Tree with initial rows (Alice, Bob, Charlie).\n";
    
    // Demonstrate Index Scan
    std::cout << "Running IndexScanExecutor for ID = 2...\n";
    IndexScanExecutor idxScan(&context, &index, 2);
    idxScan.init();
    
    Row resultRow;
    if (idxScan.next(&resultRow)) {
        std::cout << "-> Found Row: ID=" << resultRow.getValues()[0].intValue 
                  << ", Name=" << resultRow.getValues()[1].stringValue << "\n";
    } else {
        std::cout << "-> Row not found.\n";
    }

    // Start REPL
    REPL repl(&context, schema);
    repl.start();

    return 0;
}
