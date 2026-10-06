cmake_minimum_required(VERSION 3.10)
project(CppDatabaseEngine)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add our source files
set(SOURCES
    src/StorageManager.cpp
    src/Row.cpp
    src/ExecutionEngine.cpp
    src/Expression.cpp
    src/REPL.cpp
    main.cpp
)

# Create an executable
add_executable(db_engine ${SOURCES})

# Include directories so compiler can find our .h files
target_include_directories(db_engine PRIVATE include)

# Enable warnings
target_compile_options(db_engine PRIVATE -Wall -Wextra)