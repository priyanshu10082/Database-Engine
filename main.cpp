#include <iostream>
#include <vector>
#include "include/StorageManager.h"
#include "include/Schema.h"
#include "include/Row.h"
#include "include/ExecutionEngine.h"
#include "include/BPlusTree.h"
#include "include/REPL.h"

// Scans all rows from disk and repopulates the B+ tree
void rebuildIndex(ExecutionContext& context, Schema& schema, 
                  BPlusTree<int, Row>& index, int totalRows) {
    if (totalRows == 0) return;

    SeqScanExecutor scanner(&context, schema, 1, totalRows);  // page 1 — data starts here
    scanner.init();
    Row row;
    while (scanner.next(&row)) {
        int id = row.getValues()[0].intValue;
        index.insert(id, row);
    }
    std::cout << "Rebuilt index: " << totalRows << " rows loaded." << std::endl;
}

int main() {
    std::cout << "Initializing Database Engine with B+ Tree Index..." << std::endl;

    std::vector<Column> columns = {
        {"id", DataType::INT, 4},
        {"name", DataType::VARCHAR, 32}
    };
    Schema schema(columns);

    StorageManager storage("test_db.bin");
    ExecutionContext context;
    context.storage = &storage;

    BPlusTree<int, Row> index(4);

    // Read how many rows exist from the header page
    int savedRows = storage.readMetadata();

    // Rebuild B+ tree from disk before starting REPL
    rebuildIndex(context, schema, index, savedRows);

    REPL repl(&context, schema, &index);
    repl.start();

    return 0;
}