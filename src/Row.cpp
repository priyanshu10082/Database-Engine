#include "../include/Row.h"
#include <cstring>
#include <stdexcept>
#include <algorithm>

void Row::serialize(char* destination, const Schema& schema) const {
    const auto& columns = schema.getColumns();
    if (columns.size() != values.size()) {
        throw std::runtime_error("Row value count does not match schema column count.");
    }

    uint32_t offset = 0;
    for (size_t i = 0; i < columns.size(); ++i) {
        const auto& col = columns[i];
        const auto& val = values[i];

        if (col.type == DataType::INT) {
            std::memcpy(destination + offset, &val.intValue, sizeof(int));
        } else if (col.type == DataType::VARCHAR) {
            uint32_t lengthToCopy = std::min(static_cast<uint32_t>(val.stringValue.length()), col.size);
            
            // First, zero out the memory to ensure no garbage data for shorter strings
            std::memset(destination + offset, 0, col.size);
            
            // Then copy the actual string bytes
            std::memcpy(destination + offset, val.stringValue.c_str(), lengthToCopy);
        }

        offset += col.size;
    }
}

void Row::deserialize(const char* source, const Schema& schema) {
    const auto& columns = schema.getColumns();
    values.clear();

    uint32_t offset = 0;
    for (const auto& col : columns) {
        if (col.type == DataType::INT) {
            int intVal;
            std::memcpy(&intVal, source + offset, sizeof(int));
            values.push_back(DBValue(intVal));
        } else if (col.type == DataType::VARCHAR) {
            // Read until null terminator or max size
            std::string strVal;
            for (uint32_t i = 0; i < col.size; ++i) {
                char c = *(source + offset + i);
                if (c == '\0') break;
                strVal += c;
            }
            values.push_back(DBValue(strVal));
        }

        offset += col.size;
    }
}
