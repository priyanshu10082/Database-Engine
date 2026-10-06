#pragma once
#include "Page.h"
#include <string>
#include <fstream>

// The StorageManager handles reading and writing Pages to the physical hard drive.
class StorageManager {
private:
    std::string filename;
    std::fstream fileStream;

public:
    // Constructor: Opens the file, creates it if it doesn't exist.
    StorageManager(const std::string& dbFilename);

    // Destructor: Closes the file
    ~StorageManager();

    // Read a specific page from disk into memory
    bool readPage(int pageId, Page& page);

    // Write a page from memory back to disk
    bool writePage(int pageId, const Page& page);

    // Write totalRows into page 0 (metadata/header page)
    void writeMetadata(int totalRows);

    // Read totalRows from page 0 (metadata/header page)
    int readMetadata();
};