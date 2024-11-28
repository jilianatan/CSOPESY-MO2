#include "PagingAllocator.h"
#include <algorithm>
#include <queue>

// Constructor
PagingAllocator::PagingAllocator(int total_memory, int frame_size)
    : frame_size(frame_size), total_frames(total_memory / frame_size) {
    frame_usage.resize(total_frames, false); // All frames initially free
}

// Allocates memory for a process
bool PagingAllocator::allocate(int process_id, int size) {
    int num_pages = (size + frame_size - 1) / frame_size; // Round up to nearest page
    std::vector<PageTableEntry> page_table;

    for (int i = 0; i < num_pages; ++i) {
        // Find a free frame
        auto it = std::find(frame_usage.begin(), frame_usage.end(), false);
        if (it == frame_usage.end()) {
            // No free frame found, swap out the oldest process
            swapOutOldest();
            it = std::find(frame_usage.begin(), frame_usage.end(), false);
            if (it == frame_usage.end()) {
                // Still no free frame, fail allocation
                return false;
            }
        }

        int frame_index = it - frame_usage.begin();
        frame_usage[frame_index] = true;

        // Add page table entry
        page_table.push_back({ frame_index, true });
    }

    page_tables[process_id] = page_table; // Save the page table for the process
    return true;
}

// Deallocates memory for a process
void PagingAllocator::deallocate(int process_id) {
    for (const auto& entry : page_tables[process_id]) {
        if (entry.valid) {
            frame_usage[entry.frame_number] = false; // Mark frame as free
        }
    }
    page_tables.erase(process_id); // Remove the process's page table
}

// Loads a page into memory
void PagingAllocator::pageIn(int process_id, int page_number) {
    if (page_tables.find(process_id) == page_tables.end()) return;
    if (page_number >= page_tables[process_id].size()) return;

    auto& entry = page_tables[process_id][page_number];
    if (!entry.valid) {
        // Find a free frame
        auto it = std::find(frame_usage.begin(), frame_usage.end(), false);
        if (it == frame_usage.end()) {
            // No free frame, swap out the oldest process
            swapOutOldest();
            it = std::find(frame_usage.begin(), frame_usage.end(), false);
            if (it == frame_usage.end()) {
                // Still no free frame, cannot page in
                std::cerr << "No free frame available for paging in process " << process_id << "\n";
                return;
            }
        }

        int frame_index = it - frame_usage.begin();
        frame_usage[frame_index] = true;
        entry.frame_number = frame_index;
        entry.valid = true;
        ++pages_paged_in;
    }
}

// Removes a page from memory
void PagingAllocator::pageOut(int process_id, int page_number) {
    if (page_tables.find(process_id) == page_tables.end()) return;
    if (page_number >= page_tables[process_id].size()) return;

    auto& entry = page_tables[process_id][page_number];
    if (entry.valid) {
        frame_usage[entry.frame_number] = false;
        entry.valid = false;
        ++pages_paged_out;
    }
}

// Swaps out the oldest process to backing store
void PagingAllocator::swapOutOldest() {
    if (backing_store.empty()) return;

    int oldest_process_id = backing_store.front();
    backing_store.pop();

    deallocate(oldest_process_id);
}

// Prints the current memory state
void PagingAllocator::printMemoryState() {
    size_t used_memory = 0;
    size_t free_memory = total_frames * frame_size;

    for (const auto& frame : frame_usage) {
        if (frame) {
            used_memory += frame_size;
            free_memory -= frame_size;
        }
    }

    std::cout << "Used memory: " << used_memory << " KB\n";
    std::cout << "Free memory: " << free_memory << " KB\n";
    std::cout << "Pages paged in: " << pages_paged_in << "\n";
    std::cout << "Pages paged out: " << pages_paged_out << "\n";
}

// Prints a high-level overview of memory allocation
void PagingAllocator::printProcessSMI() {
    std::cout << "Process SMI:\n";
    for (const auto& entry : page_tables) {
        std::cout << "Process ID: " << entry.first << ", Pages: " << entry.second.size() << "\n";
    }
}

// Prints fine-grained memory details
void PagingAllocator::printVMStat() {
    std::cout << "VMStat:\n";
    for (const auto& entry : page_tables) {
        std::cout << "Process ID: " << entry.first << "\n";
        for (const auto& page : entry.second) {
            std::cout << "  Page Frame: " << page.frame_number << ", Valid: " << page.valid << "\n";
        }
    }
}
