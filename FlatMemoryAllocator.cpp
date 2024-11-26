#include "FlatMemoryAllocator.h"

// Constructor
FlatMemoryAllocator::FlatMemoryAllocator(int total_memory)
    : total_memory(total_memory) {
    memory_blocks.push_back({ 0, total_memory, true, -1 }); // Initial block
}

// Allocates memory for a process
int FlatMemoryAllocator::allocate(int process_id, int size) {
    for (auto& block : memory_blocks) {
        if (block.free && block.size >= size) {
            int remaining = block.size - size;
            block.free = false;
            block.size = size;
            block.process_id = process_id;

            // Create a new free block if there's remaining space
            if (remaining > 0) {
                memory_blocks.insert(
                    memory_blocks.begin() + (&block - &memory_blocks[0]) + 1,
                    { block.start + size, remaining, true, -1 });
            }

            return block.start; // Return starting address
        }
    }

    // No suitable block found, trigger swapping
    swapOutOldest();
    return allocate(process_id, size); // Retry allocation
}

// Frees memory used by a process
void FlatMemoryAllocator::deallocate(int process_id) {
    for (auto it = memory_blocks.begin(); it != memory_blocks.end(); ++it) {
        if (!it->free && it->process_id == process_id) {
            it->free = true;
            it->process_id = -1;

            // Merge adjacent free blocks
            if (it != memory_blocks.begin() && std::prev(it)->free) {
                auto prev = std::prev(it);
                prev->size += it->size;
                it = memory_blocks.erase(it); // Erase current block and update iterator
                --it; // Move iterator back to previous block
            }
            if (it != std::prev(memory_blocks.end()) && std::next(it)->free) {
                auto next = std::next(it);
                it->size += next->size;
                memory_blocks.erase(next);
            }
            return; // Exit once the block is deallocated and merged
        }
    }

}

// Swaps out the oldest process to the backing store
void FlatMemoryAllocator::swapOutOldest() {
    if (!memory_blocks.empty()) {
        for (auto& block : memory_blocks) {
            if (!block.free) {
                backing_store.push(block.process_id);
                deallocate(block.process_id);
                break;
            }
        }
    }
}

// Prints the current memory state
void FlatMemoryAllocator::printMemoryState() {
    std::cout << "Memory State:\n";
    for (const auto& block : memory_blocks) {
        std::cout << "[" << block.start << " - " << block.start + block.size - 1 << "] ";
        std::cout << (block.free ? "Free" : "Occupied by Process " + std::to_string(block.process_id)) << "\n";
    }
    std::cout << "-----------------------------\n";
}

// Calculates external fragmentation
int FlatMemoryAllocator::calculateExternalFragmentation() {
    int fragmentation = 0;
    for (const auto& block : memory_blocks) {
        if (block.free) {
            fragmentation += block.size;
        }
    }
    return fragmentation;
}
