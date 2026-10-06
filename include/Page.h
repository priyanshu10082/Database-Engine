#pragma once
#include <array>
#include <cstdint>
#include <cstddef>

// A Page is a fixed-size block of memory.
// This is the smallest unit of data we read from or write to the hard drive.
constexpr size_t PAGE_SIZE = 4096; // 4 KB pages are standard

class Page {
public:
    // The raw bytes of the page
    std::array<char, PAGE_SIZE> data;

    Page() {
        data.fill(0); // Initialize the page with zeros
    }
};
