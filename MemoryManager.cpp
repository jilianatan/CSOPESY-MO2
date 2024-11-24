#include "MemoryManager.h"
#include "Config.h"
#include <iostream>
#include <vector>
#include <map>
#include <iomanip>


MemoryManager::MemoryManager()
    : maxOverallMem(0), memPerFrame(0), usedMemory(0), freeMemory(0), totalFrames(0) {
}

void MemoryManager::initialize(size_t maxMem, size_t frameSize) {
    // Assign parameters
    maxOverallMem = maxMem;
    memPerFrame = frameSize;

    if (memPerFrame == 0) {
        throw std::invalid_argument("mem-per-frame must not be zero.");
    }

    if (maxOverallMem % memPerFrame != 0) {
        throw std::invalid_argument("max-overall-mem must be a multiple of mem-per-frame.");
    }

    totalFrames = maxOverallMem / memPerFrame;
    frameUsage.resize(totalFrames, false);
    freeMemory = maxOverallMem;

}



bool MemoryManager::allocateMemory(int processId, size_t memSize) {
    size_t framesNeeded = (memSize + memPerFrame - 1) / memPerFrame;

    if (framesNeeded > freeMemory / memPerFrame) {
        numPagedOut++; // Simulate a page out when memory allocation fails
        incrementIdleTicks(1); // Increment idle ticks due to failed allocation
        return false;
    }

    std::vector<int> allocatedFrames;
    for (size_t i = 0; i < frameUsage.size() && framesNeeded > 0; ++i) {
        if (!frameUsage[i]) {
            frameUsage[i] = true;
            allocatedFrames.push_back(i);
            --framesNeeded;
        }
    }

    processPageTable[processId] = allocatedFrames;
    size_t allocatedMem = allocatedFrames.size() * memPerFrame;
    usedMemory += allocatedMem;
    freeMemory -= allocatedMem;

    numPagedIn++; // Simulate a page in for each successful allocation
    incrementActiveTicks(allocatedFrames.size()); // Increment active ticks for frames allocated

    return true;
}


void MemoryManager::deallocateMemory(int processId) {
    if (processPageTable.find(processId) == processPageTable.end()) {
        std::cout << "No memory allocated for process " << processId << ".\n";
        incrementIdleTicks(1); // Increment idle ticks for invalid deallocation attempt
        return;
    }

    for (int frame : processPageTable[processId]) {
        frameUsage[frame] = false;
    }

    size_t memFreed = processPageTable[processId].size() * memPerFrame;
    usedMemory -= memFreed;
    freeMemory += memFreed;

    processPageTable.erase(processId);

    std::cout << "Deallocated " << memFreed << " KB from process " << processId << ".\n";
}


void MemoryManager::displayMemoryState() {
    std::cout << "\n===== Memory State =====\n";
    std::cout << "Total Memory: " << maxOverallMem << " KB\n";
    std::cout << "Used Memory: " << usedMemory << " KB\n";
    std::cout << "Free Memory: " << freeMemory << " KB\n";

    std::cout << "\nProcesses in Memory:\n";
    if (processPageTable.empty()) {
        std::cout << "No processes are currently allocated memory.\n";
    }
    else {
        for (const auto& entry : processPageTable) {
            int processId = entry.first; // Process ID
            const std::vector<int>& frames = entry.second; // Frames allocated to this process

            std::cout << "Process ID: " << processId << " | Memory Allocated: "
                << frames.size() * memPerFrame << " KB | Frames: ";
            for (int frame : frames) {
                std::cout << frame << " ";
            }
            std::cout << "\n";
        }
    }
    std::cout << "=========================\n";
}

void MemoryManager::incrementIdleTicks(size_t ticks) {
    idleCpuTicks += ticks;
    totalCpuTicks += ticks;
}

void MemoryManager::incrementActiveTicks(size_t ticks) {
    activeCpuTicks += ticks;
    totalCpuTicks += ticks;
}


void MemoryManager::displayDetailedStats() {
    std::cout << "\n===== VMSTAT Report =====\n";
    std::cout << "Total Memory: " << maxOverallMem << " KB\n";
    std::cout << "Used Memory: " << usedMemory << " KB\n";
    std::cout << "Free Memory: " << freeMemory << " KB\n";
    std::cout << "Idle CPU Ticks: " << idleCpuTicks << "\n";
    std::cout << "Active CPU Ticks: " << activeCpuTicks << "\n";
    std::cout << "Total CPU Ticks: " << totalCpuTicks << "\n";
    std::cout << "Pages Paged In: " << numPagedIn << "\n";
    std::cout << "Pages Paged Out: " << numPagedOut << "\n";
    std::cout << "=========================\n";
}
