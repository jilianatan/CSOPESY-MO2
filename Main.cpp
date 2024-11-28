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

    const auto& config = Config::GetConfigParameters();

    // Initialize scheduler
    FCFS_Scheduler fcfs_scheduler(0, config.max_overall_mem);
    RR_Scheduler rr_scheduler(0, 0, config.max_overall_mem);

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

            // Choose memory allocator
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

            // Initialize the scheduler
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
        else if (tokens[0] == "scheduler-test") {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(config.min_ins, config.max_ins);

            if (!scheduler_testing) {
                scheduler_testing = true;
                scheduler_thread = std::thread([&]() {
                    while (scheduler_testing) {
                        int commands_per_process = dist(gen);

                        if (Config::GetConfigParameters().scheduler == "fcfs") {
                            fcfs_scheduler.add_process(new Process("process" + std::to_string(++process_count), commands_per_process, config.max_overall_mem));
                        }
                        else if (Config::GetConfigParameters().scheduler == "rr") {
                            rr_scheduler.add_process(new Process("process" + std::to_string(++process_count), commands_per_process, config.max_overall_mem));
                        }

                        std::this_thread::sleep_for(std::chrono::milliseconds((int)(config.batch_process_freq * 1000)));
                    }
                    });
                scheduler_thread.detach();

                std::cout << "Scheduler test execution started.\n";
            }
            else {
                std::cout << "Scheduler test is already running.\n";
            }
        }
        else if (tokens[0] == "process-smi") {
            if (is_paging && paging_allocator) {
                paging_allocator->printMemoryState();
            }
            else if (flat_allocator) {
                flat_allocator->printMemoryState();
            }
        }
        else if (tokens[0] == "screen" && tokens[1] == "-ls") {
            if (config.scheduler == "fcfs") {
                fcfs_scheduler.screen_ls();
            }
            else if (config.scheduler == "rr") {
                rr_scheduler.screen_ls();
            }
        }
        else if (tokens[0] == "vmstat") {
            std::cout << "Total memory: " << config.max_overall_mem << " KB\n";
            std::cout << "Used memory: " << config.max_overall_mem << " KB\n";
            std::cout << "Free memory: 0 KB\n";

            size_t idle_ticks = 0;
            size_t active_ticks = 0;

            if (config.scheduler == "fcfs") {
                idle_ticks = fcfs_scheduler.getIdleTicks();
                active_ticks = fcfs_scheduler.getActiveTicks();
            }
            else if (config.scheduler == "rr") {
                idle_ticks = rr_scheduler.getIdleTicks();
                active_ticks = rr_scheduler.getActiveTicks();
            }

            std::cout << "Idle CPU ticks: " << idle_ticks << "\n";
            std::cout << "Active CPU ticks: " << active_ticks << "\n";
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
