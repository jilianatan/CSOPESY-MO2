#ifndef FLATMEMORYALLOCATOR_H
#define FLATMEMORYALLOCATOR_H

#include <vector>
#include <cstddef>

struct MemoryBlock {
    size_t start_address;
    size_t size;
    bool free;
    int process_id;

    MemoryBlock(size_t start, size_t sz) : start_address(start), size(sz), free(true), process_id(-1) {}
};

class FlatMemoryAllocator {
public:
    FlatMemoryAllocator(size_t total_memory);
    int allocate(int process_id, size_t size);
    void deallocate(int process_id);
    void printMemoryState() const;
    std::vector<MemoryBlock> getMemoryBlocks() const;

private:
    size_t total_memory;
    std::vector<MemoryBlock> memory_blocks;
};

#endif // FLATMEMORYALLOCATOR_H
