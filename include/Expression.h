#pragma once
#include "Row.h"
#include "Schema.h"
#include "Types.h"
#include <string>

// AbstractExpression: Evaluates a condition against a Row
class AbstractExpression {
public:
    virtual ~AbstractExpression() = default;
    virtual bool evaluate(const Row& row, const Schema& schema) const = 0;
};

// ComparisonExpression: Compares a specific column against a constant value (equality only for now)
class ComparisonExpression : public AbstractExpression {
private:
    uint32_t columnIndex;
    DBValue value;

public:
    ComparisonExpression(uint32_t colIdx, const DBValue& val);
    bool evaluate(const Row& row, const Schema& schema) const override;
};
