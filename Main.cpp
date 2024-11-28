#include<iostream>
#include <string>
#include "ConsoleManager.h"
#include "Commands.h"
#include "FCFSScheduler.h"
#include "Console.h"
#include "Config.h"
#include "RRScheduler.h"
#include "PagingAllocator.h"
#include "FlatMemoryAllocator.h"
#include <random>
#include <thread>
#include <sstream>
#include <vector>
#include <memory>

int main() {
    displayHeader();
    std::string command;

    ConsoleManager console_manager;
    bool running = true;

    FlatMemoryAllocator* flat_allocator = nullptr;
    PagingAllocator* paging_allocator = nullptr;
    bool is_paging = false;

    // Initialize scheduler
    FCFS_Scheduler fcfs_scheduler(0, 32768); // Initialize with total memory
    RR_Scheduler rr_scheduler(0, 0, 32768); // Initialize with total memory

    // Initialize scheduling test
    std::thread scheduler_thread;
    int process_count = 0;
    bool scheduler_testing = false;
    std::string screen_process_name = "";
    bool initialized = false;

    while (running) {
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);

        // Input tokens
        std::stringstream ss(command);
        std::vector<std::string> tokens;
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }

        if (tokens.empty()) continue;

        if (tokens[0] == "initialize") {
            Config::Initialize();
            std::cout << "Config initialized with \"config.txt\" parameters" << std::endl;

            const auto& config = Config::GetConfigParameters();

            if (config.max_overall_mem == config.mem_per_frame) {
                flat_allocator = new FlatMemoryAllocator(config.max_overall_mem);
                is_paging = false;
                std::cout << "Flat memory allocator initialized.\n";
            }
            else {
                paging_allocator = new PagingAllocator(config.max_overall_mem, config.mem_per_frame);
                is_paging = true;
                std::cout << "Paging memory allocator initialized.\n";
            }

            if (config.scheduler == "fcfs") {
                fcfs_scheduler.SetCpuCore(config.num_cpu);
                fcfs_scheduler.start();
            }
            else if (config.scheduler == "rr") {
                rr_scheduler.SetCpuCore(config.num_cpu);
                rr_scheduler.SetQuantum(config.quantum_cycles);
                rr_scheduler.start();
            }
            initialized = true;
        }
        else if (tokens[0] == "screen" && tokens[1] == "-s") {
            if (!initialized) {
                std::cout << "Initialize the program with command \"initialize\"" << std::endl;
                continue;
            }

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(Config::GetConfigParameters().min_ins, Config::GetConfigParameters().max_ins);
            std::uniform_int_distribution<> mem_dist(Config::GetConfigParameters().min_mem_per_proc, Config::GetConfigParameters().max_mem_per_proc);

            int required_memory = mem_dist(gen); // Random memory requirement for the process

            if (Config::GetConfigParameters().scheduler == "fcfs") {
                if (fcfs_scheduler.isValidProcessName(tokens[2]) == false) {
                    std::cout << "Process with name \"" + tokens[2] + "\" already exists" << std::endl;
                }
                else {
                    bool allocation_success = false;
                    if (is_paging && paging_allocator) {
                        allocation_success = paging_allocator->allocate(std::stoi(tokens[2]), required_memory);
                    }
                    else if (!is_paging && flat_allocator) {
                        allocation_success = (flat_allocator->allocate(std::stoi(tokens[2]), required_memory) != -1);
                    }

                    if (!allocation_success) {
                        std::cout << "Memory allocation failed for process \"" + tokens[2] + "\"." << std::endl;
                        continue;
                    }

                    int commands_per_process = dist(gen);
                    Process* new_process = new Process(tokens[2], commands_per_process);
                    fcfs_scheduler.add_process(new_process);

                    std::shared_ptr<Console> new_console(new Console(new_process->name, new_process->executed_commands, new_process->total_commands, new_process->process_id));
                    screen_process_name = tokens[2];
                    console_manager.setCurrentConsole(new_console);
                    fcfs_scheduler.print_process_details(tokens[2], 0);
                }
            }
            else if (Config::GetConfigParameters().scheduler == "rr") {
                if (rr_scheduler.isValidProcessName(tokens[2]) == false) {
                    std::cout << "Process with name \"" + tokens[2] + "\" already exists" << std::endl;
                }
                else {
                    bool allocation_success = false;
                    if (is_paging && paging_allocator) {
                        allocation_success = paging_allocator->allocate(std::stoi(tokens[2]), required_memory);
                    }
                    else if (!is_paging && flat_allocator) {
                        allocation_success = (flat_allocator->allocate(std::stoi(tokens[2]), required_memory) != -1);
                    }

                    if (!allocation_success) {
                        std::cout << "Memory allocation failed for process \"" + tokens[2] + "\"." << std::endl;
                        continue;
                    }

                    int commands_per_process = dist(gen);
                    Process* new_process = new Process(tokens[2], commands_per_process);
                    rr_scheduler.add_process(new_process);

                    std::shared_ptr<Console> new_console(new Console(new_process->name, new_process->executed_commands, new_process->total_commands, new_process->process_id));
                    screen_process_name = tokens[2];
                    console_manager.setCurrentConsole(new_console);
                    rr_scheduler.print_process_details(tokens[2], 0);
                }
            }
        }
        else if (tokens[0] == "vmstat") {
            const auto& config = Config::GetConfigParameters();
            size_t used_memory = 0;
            size_t free_memory = 0;

            if (is_paging && paging_allocator) {
                const auto& frame_usage = paging_allocator->getFrameUsage();
                for (size_t i = 0; i < frame_usage.size(); ++i) {
                    if (frame_usage[i]) {
                        used_memory += config.mem_per_frame;
                    }
                    else {
                        free_memory += config.mem_per_frame;
                    }
                }
            }
            else if (!is_paging && flat_allocator) {
                for (const auto& block : flat_allocator->getMemoryBlocks()) {
                    if (!block.free) {
                        used_memory += block.size;
                    }
                }
                free_memory = config.max_overall_mem - used_memory;
            }

            std::cout << "Total memory: " << config.max_overall_mem << " KB\n";
            std::cout << "Used memory: " << used_memory << " KB\n";
            std::cout << "Free memory: " << free_memory << " KB\n";

            // Call vmstat for the appropriate scheduler
            if (config.scheduler == "fcfs") {
                fcfs_scheduler.vmstat();
            }
            else if (config.scheduler == "rr") {
                rr_scheduler.vmstat();
            }
        }
        else if (command == "exit") {
            running = false;
        }
    }

    if (scheduler_thread.joinable()) {
        scheduler_thread.join();
    }

    delete flat_allocator;
    delete paging_allocator;

    return 0;
}