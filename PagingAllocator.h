#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>

struct PageTableEntry {
    int frame_number; // The frame index
    bool valid;       // Valid bit indicating if the page is in memory
};

class PagingAllocator {
private:
    size_t pages_paged_in = 0;  // Number of pages paged into memory
    size_t pages_paged_out = 0; // Number of pages paged out of memory
    int total_frames;                   // Total number of frames
    int frame_size;                     // Size of each frame in KB
    std::vector<bool> frame_usage;      // Frame occupancy
    std::unordered_map<int, std::vector<PageTableEntry>> page_tables; // Page tables for processes

public:
    PagingAllocator(int total_memory, int frame_size);

    const std::vector<bool>& getFrameUsage() const {
        return frame_usage;
    }

    size_t getPagesPagedIn() const { return pages_paged_in; }
    size_t getPagesPagedOut() const { return pages_paged_out; }


    bool allocate(int process_id, int size);  // Allocates memory for a process
    void deallocate(int process_id);          // Deallocates memory for a process
    void pageIn(int process_id, int page_number);  // Loads a page into memory
    void pageOut(int process_id, int page_number); // Removes a page from memory
    void printMemoryState();                  // Prints the current memory state
};
