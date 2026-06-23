#pragma once
#include <string>

// Fixed set of name constants
enum class DataType { // SQLite's early design
    INT,
    VARCHAR
};

// A value in our database can be an int or a string
// Using a struct instead of std::variant for compatibility with GCC 6.3
struct DBValue {
    DataType type;
    int intValue;
    std::string stringValue;

    DBValue() : type(DataType::INT), intValue(0) {}
    DBValue(int val) : type(DataType::INT), intValue(val) {}
    DBValue(const std::string& val) : type(DataType::VARCHAR), intValue(0), stringValue(val) {}
    DBValue(const char* val) : type(DataType::VARCHAR), intValue(0), stringValue(val) {}
};
