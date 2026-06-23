#include "Expression.h"

ComparisonExpression::ComparisonExpression(uint32_t colIdx, const DBValue& val) 
    : columnIndex(colIdx), value(val) {}

bool ComparisonExpression::evaluate(const Row& row, const Schema& schema) const {
    const auto& values = row.getValues();
    if (columnIndex >= values.size()) {
        return false; // Out of bounds
    }

    const DBValue& rowValue = values[columnIndex];
    
    // We only support simple equality comparison for now
    if (rowValue.type != value.type) return false;

    if (rowValue.type == DataType::INT) {
        return rowValue.intValue == value.intValue;
    } else {
        return rowValue.stringValue == value.stringValue;
    }
}
