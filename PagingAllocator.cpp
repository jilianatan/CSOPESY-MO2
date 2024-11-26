#include "PagingAllocator.h"

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
            // No free frame found, fail allocation
            return false;
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
    if (page_tables.find(process_id) == page_tables.end()) return;

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
            // No free frame, cannot page in
            std::cerr << "No free frame available for paging in process " << process_id << "\n";
            return;
        }

        int frame_index = it - frame_usage.begin();
        frame_usage[frame_index] = true;
        entry.frame_number = frame_index;
        entry.valid = true;

        pages_paged_in++; // Increment the counter
        std::cout << "Paged in process " << process_id << ", page " << page_number
            << " into frame " << frame_index << "\n";
    }
}



// Removes a page from memory
void PagingAllocator::pageOut(int process_id, int page_number) {
    if (page_tables.find(process_id) == page_tables.end()) return;
    if (page_number >= page_tables[process_id].size()) return;

    auto& entry = page_tables[process_id][page_number];
    if (entry.valid) {
        frame_usage[entry.frame_number] = false; // Free the frame
        entry.valid = false;

        pages_paged_out++; // Increment the counter
        std::cout << "Paged out process " << process_id << ", page " << page_number
            << " from frame " << entry.frame_number << "\n";
    }
}


// Prints the current memory state
void PagingAllocator::printMemoryState() {
    std::cout << "Paging Memory State:\n";
    for (size_t i = 0; i < frame_usage.size(); ++i) {
        std::cout << "Frame " << i << ": " << (frame_usage[i] ? "Occupied" : "Free") << "\n";
    }

    std::cout << "Page Tables:\n";
    for (const auto& pair : page_tables) {
        int process_id = pair.first;                      // Extract process ID
        const auto& page_table = pair.second;             // Extract page table

        std::cout << "Process " << process_id << ":\n";
        for (size_t i = 0; i < page_table.size(); ++i) {
            std::cout << "  Page " << i << " -> Frame " << page_table[i].frame_number
                << " (" << (page_table[i].valid ? "Valid" : "Invalid") << ")\n";
        }
    }
    std::cout << "-----------------------------\n";
}

