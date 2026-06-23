#pragma once
#include "Types.h"
#include <string>
#include <vector>
#include <cstdint>

struct Column {
    std::string name;
    DataType type;
    uint32_t size; // Fixed size in bytes
};

class Schema {
private:
    std::vector<Column> columns;

public:
    Schema(const std::vector<Column>& cols) : columns(cols) {}

    const std::vector<Column>& getColumns() const {
        return columns;
    }

    uint32_t getRowSize() const {
        uint32_t totalSize = 0;
        for (const auto& col : columns) {
            totalSize += col.size;
        }
        return totalSize;
    }
};
