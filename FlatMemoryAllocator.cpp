#include "FlatMemoryAllocator.h"
#include <iostream>

FlatMemoryAllocator::FlatMemoryAllocator(size_t total_memory) : total_memory(total_memory) {
    // Initialize memory blocks with a single large free block
    memory_blocks.emplace_back(0, total_memory);
}

int FlatMemoryAllocator::allocate(int process_id, size_t size) {
    // Implement allocation logic
    for (auto& block : memory_blocks) {
        if (block.free && block.size >= size) {
            if (block.size > size) {
                // Split the block
                memory_blocks.emplace_back(block.start_address + size, block.size - size);
            }
            block.size = size;
            block.free = false;
            block.process_id = process_id;
            return block.start_address;
        }
    }
    return -1; // Allocation failed
}

void FlatMemoryAllocator::deallocate(int process_id) {
    // Implement deallocation logic
    for (auto& block : memory_blocks) {
        if (block.process_id == process_id) {
            block.free = true;
            block.process_id = -1;
            // Merge adjacent free blocks
            for (auto it = memory_blocks.begin(); it != memory_blocks.end(); ++it) {
                if (it->free && (it + 1) != memory_blocks.end() && (it + 1)->free) {
                    it->size += (it + 1)->size;
                    memory_blocks.erase(it + 1);
                    --it;
                }
            }
            break;
        }
    }
}

void FlatMemoryAllocator::printMemoryState() const {
    // Implement memory state printing logic
    std::cout << "Memory State:\n";
    for (const auto& block : memory_blocks) {
        std::cout << "Start Address: " << block.start_address
            << ", Size: " << block.size
            << ", Free: " << (block.free ? "Yes" : "No")
            << ", Process ID: " << block.process_id << "\n";
    }
}

std::vector<MemoryBlock> FlatMemoryAllocator::getMemoryBlocks() const {
    return memory_blocks;
}
