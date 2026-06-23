#include "ExecutionEngine.h"

// --- SeqScanExecutor ---
SeqScanExecutor::SeqScanExecutor(ExecutionContext* ctx, const Schema& schema, int startPageId, int totalRows)
    : AbstractExecutor(ctx), schema(schema), startPageId(startPageId), totalRowsToScan(totalRows) {
    uint32_t rowSize = schema.getRowSize();
    rowsPerPage = PAGE_SIZE / rowSize;
}

void SeqScanExecutor::init() {
    rowsScanned = 0;
    currentPageId = startPageId;
    currentRowInPage = 0;
    
    // Read the first page
    if (totalRowsToScan > 0) {
        context->storage->readPage(currentPageId, currentPage);
    }
}

bool SeqScanExecutor::next(Row* row) {
    if (rowsScanned >= totalRowsToScan) {
        return false; // EOF
    }

    if (currentRowInPage >= rowsPerPage) {
        // Move to next page
        currentPageId++;
        currentRowInPage = 0;
        context->storage->readPage(currentPageId, currentPage);
    }

    // Deserialize the row from the current page
    uint32_t rowSize = schema.getRowSize();
    uint32_t offset = currentRowInPage * rowSize;
    
    if (row) {
        row->deserialize(currentPage.data.data() + offset, schema);
    }

    currentRowInPage++;
    rowsScanned++;
    return true;
}

// --- InsertExecutor ---
InsertExecutor::InsertExecutor(ExecutionContext* ctx, const Schema& schema, const std::vector<Row>& rows, int startPageId)
    : AbstractExecutor(ctx), schema(schema), rowsToInsert(rows), startPageId(startPageId) {
    uint32_t rowSize = schema.getRowSize();
    rowsPerPage = PAGE_SIZE / rowSize;
}

void InsertExecutor::init() {
    rowsInserted = 0;
    currentPageId = startPageId;
    currentRowInPage = 0;
    
    currentPage = Page(); // Reset page
}

bool InsertExecutor::next(Row* row) {
    if (rowsInserted >= rowsToInsert.size()) {
        // Flush the last page if there are pending rows and we haven't flushed yet
        if (currentRowInPage > 0) {
            context->storage->writePage(currentPageId, currentPage);
            currentRowInPage = 0; // Prevent writing multiple times on subsequent next() calls
        }
        return false; // Done
    }

    if (currentRowInPage >= rowsPerPage) {
        // Flush current page and move to next
        context->storage->writePage(currentPageId, currentPage);
        currentPageId++;
        currentRowInPage = 0;
        currentPage = Page(); // Reset page
    }

    // Serialize row into page
    const Row& rowToInsert = rowsToInsert[rowsInserted];
    uint32_t rowSize = schema.getRowSize();
    uint32_t offset = currentRowInPage * rowSize;
    rowToInsert.serialize(currentPage.data.data() + offset, schema);

    if (row) {
        *row = rowToInsert; // Return the inserted row
    }

    currentRowInPage++;
    rowsInserted++;
    return true;
}

// --- FilterExecutor ---
FilterExecutor::FilterExecutor(ExecutionContext* ctx, std::unique_ptr<AbstractExecutor> child, 
                               std::unique_ptr<AbstractExpression> predicate, const Schema& schema)
    : AbstractExecutor(ctx), child(std::move(child)), predicate(std::move(predicate)), schema(schema) {}

void FilterExecutor::init() {
    child->init();
}

bool FilterExecutor::next(Row* row) {
    while (child->next(row)) {
        if (predicate->evaluate(*row, schema)) {
            return true;
        }
    }
    return false; // EOF
}
