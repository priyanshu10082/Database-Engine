# C++ Mini Relational Database Engine

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue)
![CMake](https://img.shields.io/badge/CMake-3.10+-green)

A lightweight **relational database engine** built from scratch in **C++17** to explore the internal architecture of modern database systems. The project implements **page-based storage**, a **Volcano-style iterator execution engine**, and **B+ Tree indexing**, while emphasizing modular software design, extensibility, and core database internals.

---

## Table of Contents

* [Architecture](#architecture)
* [Features](#features)
* [Storage Layout](#storage-layout)
* [Row Serialization](#row-serialization)
* [Volcano Iterator Model](#volcano-iterator-model)
* [Sequential Scan](#sequential-scan)
* [Indexed Lookup](#indexed-lookup)
* [B+ Tree Structure](#b-tree-structure)
* [Executor Hierarchy](#executor-hierarchy)
* [Project Structure](#project-structure)
* [Build](#build)
* [Run](#run)
* [Example Session](#example-session)
* [Time Complexity](#time-complexity)
* [Database Concepts Demonstrated](#database-concepts-demonstrated)
* [Roadmap](#roadmap)

---

## Architecture

```text
                              REPL
                                │
                                ▼
                   Query Execution Engine
                                │
        ┌───────────────────────┴────────────────────────┐
        ▼                                                ▼
 SeqScanExecutor                                  IndexScanExecutor
        │                                                │
        ▼                                                ▼
 FilterExecutor                                    B+ Tree Index
        │                                                │
        └───────────────────────┬────────────────────────┘
                                ▼
                         Storage Manager
                                │
                                ▼
                       Database File (.db)
```

---

## Features

### Storage Layer

* Fixed-size **4096-byte page** storage architecture.
* Persistent storage using **binary serialization/deserialization**.
* Schema-aware row layout supporting multiple data types.
* File-backed **Storage Manager** responsible for reading and writing database pages.

### Query Execution Engine

Implements the **Volcano Iterator Model**, where every physical operator exposes a common

```cpp
init();
next();
```

interface.

Implemented executors:

* **SeqScanExecutor** – Sequentially scans all stored rows.
* **InsertExecutor** – Inserts records into database pages.
* **FilterExecutor** – Evaluates predicates and filters tuples.
* **IndexScanExecutor** – Performs indexed lookups using the B+ Tree.

The modular executor hierarchy enables composable query pipelines.

### B+ Tree Index

* Generic **BPlusTree<K, V>** implementation.
* Supports:

  * Insert
  * Point Lookup
  * Dynamic Node Splitting
* Integrated with the execution engine through **IndexScanExecutor**, allowing indexed queries to avoid full table scans.

### Interactive REPL

Supports:

* Record insertion
* Sequential table scans
* Predicate filtering
* Indexed lookups

---

## Storage Layout

```text
Database File (.db)

│
├── Page 0   (4096 bytes)
├── Page 1   (4096 bytes)
├── Page 2   (4096 bytes)
└── Page N   (4096 bytes)

Each page stores serialized rows according to the table schema.
```

---

## Row Serialization

Example Row

```text
ID   = 1
Name = Alice
```

Schema

```text
ID   → INT
Name → VARCHAR
```

Serialized Representation

```text
┌──────────────┬────────────────────────────┐
│ INT (4 B)    │ VARCHAR (Fixed Length)     │
└──────────────┴────────────────────────────┘
```

Rows are serialized before being written to disk and reconstructed during reads.

---

## Volcano Iterator Model

```text
FilterExecutor
        │ next()
        ▼
SeqScanExecutor
        │ next()
        ▼
StorageManager
        │ readPage()
        ▼
Database File
```

Each executor produces **one tuple at a time**, allowing physical operators to be composed into efficient query pipelines.

---

## Sequential Scan

```text
Start
  │
  ▼
Read Page 0
  │
  ▼
Scan all rows
  │
  ▼
Read Page 1
  │
  ▼
Scan all rows
  │
  ▼
...
  │
  ▼
End
```

---

## Indexed Lookup

```text
SELECT WHERE id = 42

        │
        ▼
IndexScanExecutor
        │
        ▼
B+ Tree Search
        │
        ▼
Matching Row
        │
        ▼
Return Result
```

---

## B+ Tree Structure

```text
                    [10 | 20]
                   /    |    \
                  /     |     \
         [1 5 8] [12 15] [22 30 35]
             │        │        │
             └────────┴────────┘
            Linked Leaf Nodes
```

Linked leaf nodes enable efficient range scans while internal nodes guide logarithmic-time searches.

---

## Executor Hierarchy

```text
AbstractExecutor
        │
        ├── SeqScanExecutor
        ├── InsertExecutor
        ├── FilterExecutor
        └── IndexScanExecutor
```

Every executor implements:

```cpp
init();
next();
```

allowing new physical operators to be added without changing the execution framework.

---

## Project Structure

```text
Database-Engine/
│
├── include/
│   ├── BPlusTree.h
│   ├── ExecutionEngine.h
│   ├── Expression.h
│   ├── Page.h
│   ├── REPL.h
│   ├── Row.h
│   ├── Schema.h
│   ├── StorageManager.h
│   └── Types.h
│
├── src/
│   ├── ExecutionEngine.cpp
│   ├── Expression.cpp
│   ├── REPL.cpp
│   ├── Row.cpp
│   ├── Schema.cpp
│   └── StorageManager.cpp
│
├── main.cpp
└── CMakeLists.txt
```

---

## Build

### Requirements

* C++17 compatible compiler
* CMake 3.10+

```bash
git clone https://github.com/priyanshu10082/Database-Engine.git

cd Database-Engine

mkdir build
cd build

cmake ..
cmake --build .
```

---

## Run

```bash
./db_engine
```

---

## Example Session

```text
> INSERT 1 Alice
Row inserted successfully.

> SELECT
1 Alice

> SELECT WHERE id = 1
1 Alice
```

---

## Time Complexity

| Operation       | Complexity |
| --------------- | ---------: |
| Sequential Scan |       O(n) |
| Indexed Lookup  |   O(log n) |
| B+ Tree Insert  |   O(log n) |

---

## Database Concepts Demonstrated

* Page-based storage architecture
* Binary serialization
* Persistent storage
* Storage Manager
* Volcano Iterator Model
* Runtime polymorphism
* Abstract executors
* Sequential Scan
* Predicate filtering
* B+ Tree indexing
* Modular query execution
* REPL-based database interaction

---

## Roadmap

Planned improvements include:

* Buffer Pool Manager
* Slotted Pages
* SQL Parser
* Delete Executor
* Update Executor
* Join Executor
* Transactions
* Write-Ahead Logging (WAL)
* Secondary Indexes

---

## Motivation

This project was built to gain a deeper understanding of how modern relational database systems work internally by implementing the core storage, execution, and indexing components from scratch in modern C++.
