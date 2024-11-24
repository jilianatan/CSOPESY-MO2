#pragma once

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <vector>
#include <map>
#include <cstddef>

class MemoryManager {
private:
    size_t maxOverallMem;          
    size_t memPerFrame;            
    size_t totalFrames;            
    std::vector<bool> frameUsage;  
    std::map<int, std::vector<int>> processPageTable; 

    size_t usedMemory;             
    size_t freeMemory;             

    size_t idleCpuTicks = 0;       
    size_t activeCpuTicks = 0;     
    size_t totalCpuTicks = 0;      
    size_t numPagedIn = 0;        
    size_t numPagedOut = 0;        

public:
    MemoryManager();

    void initialize(size_t maxOverallMem, size_t memPerFrame);
    bool allocateMemory(int processId, size_t memSize);
    void deallocateMemory(int processId);
    void displayMemoryState();    
    void displayDetailedStats(); 

    void incrementIdleTicks(size_t ticks = 1);
    void incrementActiveTicks(size_t ticks = 1);
};

#endif

