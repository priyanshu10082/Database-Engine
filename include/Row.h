#pragma once
#include "Types.h"
#include "Schema.h"
#include <vector>
#include <stdexcept>

class Row {
private:
    std::vector<DBValue> values;

public:
    // Constructor for creating a new row
    Row(const std::vector<DBValue>& vals) : values(vals) {}

    // Constructor for creating an empty row (e.g. before deserialization)
    Row() = default;

    const std::vector<DBValue>& getValues() const { return values; }

    // Serialize this row's data into a raw byte buffer
    void serialize(char* destination, const Schema& schema) const;

    // Deserialize a raw byte buffer back into this row's data
    void deserialize(const char* source, const Schema& schema);
};
