#include "../include/StorageManager.h"
#include <stdexcept>
#include <iostream>
#include <cstring>

StorageManager::StorageManager(const std::string& dbFilename) : filename(dbFilename) {
    fileStream.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    
    if (!fileStream.is_open()) {
        fileStream.clear();
        fileStream.open(filename, std::ios::out | std::ios::binary);
        fileStream.close();
        
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
    fileStream.clear();                                      
    fileStream.seekg(pageId * PAGE_SIZE, std::ios::beg);
    if (fileStream.read(page.data.data(), PAGE_SIZE)) {
        return true;
    }
    return false;
}

bool StorageManager::writePage(int pageId, const Page& page) {
    fileStream.clear();                                     
    fileStream.seekp(pageId * PAGE_SIZE, std::ios::beg);
    fileStream.write(page.data.data(), PAGE_SIZE);
    fileStream.flush();
    return fileStream.good();
}

void StorageManager::writeMetadata(int totalRows) {
    Page metaPage;
    std::memcpy(metaPage.data.data(), &totalRows, sizeof(int));
    writePage(0, metaPage);
}

int StorageManager::readMetadata() {
    Page metaPage;
    if (!readPage(0, metaPage)) {
        return 0;
    }
    int totalRows = 0;
    std::memcpy(&totalRows, metaPage.data.data(), sizeof(int));
    return totalRows;
}