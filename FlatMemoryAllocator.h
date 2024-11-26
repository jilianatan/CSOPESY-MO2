#pragma once
#include <vector>
#include <queue>
#include <string>
#include <iostream>

struct MemoryBlock {
    int start;  // Start address
    int size;   // Size of the block
    bool free;  // True if the block is free
    int process_id; // Process ID occupying the block (if applicable)
};

class FlatMemoryAllocator {
private:
    int total_memory;                        // Total memory size
    std::vector<MemoryBlock> memory_blocks; // List of memory blocks
    std::queue<int> backing_store;          // Process IDs in the backing store

public:
    FlatMemoryAllocator(int total_memory);

    const std::vector<MemoryBlock>& getMemoryBlocks() const {
        return memory_blocks;
    }

    int allocate(int process_id, int size);  // Allocates memory for a process
    void deallocate(int process_id);         // Frees memory used by a process
    void swapOutOldest();                    // Swaps out the oldest process to backing store
    void printMemoryState();                 // Prints the current memory state
    int calculateExternalFragmentation();    // Calculates external fragmentation
};
