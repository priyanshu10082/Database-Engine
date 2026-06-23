#include "../include/StorageManager.h"
#include <stdexcept>
#include <iostream>

StorageManager::StorageManager(const std::string& dbFilename) : filename(dbFilename) {
    // Try to open existing file for read/write
    fileStream.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    
    if (!fileStream.is_open()) {
        // If it doesn't exist, create it by opening in out mode first, then closing
        fileStream.clear();
        fileStream.open(filename, std::ios::out | std::ios::binary);
        fileStream.close();
        
        // Reopen in read/write mode
        fileStream.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!fileStream.is_open()) {
            throw std::runtime_error("Failed to open or create database file.");
        }
    }
}

StorageManager::~StorageManager() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
}

bool StorageManager::readPage(int pageId, Page& page) {
    // Seek to the correct byte offset on the disk
    fileStream.seekg(pageId * PAGE_SIZE, std::ios::beg);
    
    // Read the bytes into our Page object
    if (fileStream.read(page.data.data(), PAGE_SIZE)) {
        return true;
    }
    return false; // Could be end of file if the page hasn't been written yet
}

bool StorageManager::writePage(int pageId, const Page& page) {
    // Seek to the correct byte offset on the disk
    fileStream.seekp(pageId * PAGE_SIZE, std::ios::beg);
    
    // Write the bytes from our Page object to the file
    fileStream.write(page.data.data(), PAGE_SIZE);
    
    // Flush to ensure the OS actually saves it to the hard drive
    fileStream.flush(); 
    return fileStream.good();
}
