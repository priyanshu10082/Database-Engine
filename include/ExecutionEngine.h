#pragma once
#include "Row.h"
#include "Schema.h"
#include "StorageManager.h"
#include "Expression.h"
#include <vector>
#include <memory>

struct ExecutionContext {
    StorageManager* storage;
};

class AbstractExecutor {
protected:
    ExecutionContext* context;

public:
    AbstractExecutor(ExecutionContext* ctx) : context(ctx) {}
    virtual ~AbstractExecutor() = default;

    virtual void init() = 0;
    // Returns true if a row was found/processed, false if EOF
    virtual bool next(Row* row) = 0;
};

// SeqScanExecutor
// Scans a fixed number of rows linearly from pages.
class SeqScanExecutor : public AbstractExecutor {
private:
    Schema schema;
    int startPageId;
    int totalRowsToScan;
    
    int rowsScanned;
    int currentPageId;
    int currentRowInPage;
    int rowsPerPage;
    Page currentPage;

public:
    SeqScanExecutor(ExecutionContext* ctx, const Schema& schema, int startPageId, int totalRows);
    
    void init() override;
    bool next(Row* row) override;
};

// InsertExecutor
// Takes rows from memory (vector) and writes them to pages.
class InsertExecutor : public AbstractExecutor {
private:
    Schema schema;
    std::vector<Row> rowsToInsert;
    int startPageId;
    
    int rowsInserted;
    int currentPageId;
    int currentRowInPage;
    int rowsPerPage;
    Page currentPage;

public:
    InsertExecutor(ExecutionContext* ctx, const Schema& schema, const std::vector<Row>& rows, int startPageId);
    
    void init() override;
    bool next(Row* row) override;
};

// FilterExecutor
// Wraps a child executor and filters its output using an Expression.
class FilterExecutor : public AbstractExecutor {
private:
    std::unique_ptr<AbstractExecutor> child;
    std::unique_ptr<AbstractExpression> predicate;
    Schema schema;

public:
    FilterExecutor(ExecutionContext* ctx, std::unique_ptr<AbstractExecutor> child, 
                   std::unique_ptr<AbstractExpression> predicate, const Schema& schema);
    
    void init() override;
    bool next(Row* row) override;
};

#include "BPlusTree.h"

// IndexScanExecutor
// Uses a B+ tree to find a row by its integer primary key.
class IndexScanExecutor : public AbstractExecutor {
private:
    BPlusTree<int, Row>* bplusTree;
    int searchKey;
    bool executed; // Ensures we only return the row once

public:
    IndexScanExecutor(ExecutionContext* ctx, BPlusTree<int, Row>* tree, int key);
    
    void init() override;
    bool next(Row* row) override;
};
