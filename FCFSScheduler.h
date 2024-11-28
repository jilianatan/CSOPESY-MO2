#pragma once

#include "process.h"
#include <queue>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <list>

class FCFS_Scheduler {
private:

    std::queue<Process*> process_queue;
    std::vector<std::thread> cpu_threads;
    std::atomic<bool> running;
   // std::mutex mtx;
    std::condition_variable cv;
    int num_cores;
    std::list<Process*> running_processes;
    std::list<Process*> finished_processes;

    mutable std::mutex mtx;

    void cpu_worker(int core_id);

    size_t total_memory;
    size_t used_memory;
    size_t free_memory;

public:

    FCFS_Scheduler(int cores, size_t total_memory);
    FCFS_Scheduler(int cores);
    ~FCFS_Scheduler();

    size_t getIdleTicks() const;
    size_t getActiveTicks() const;
    size_t getTotalTicks() const;


    void add_process(Process* proc);
    void start();
    void stop();
    void screen_ls();
    void print_running_processes();
    void print_finished_processes();
    void print_process_details(const std::string& process_name, int screen);
    void print_process_queue_names();
    void SetCpuCore(int cpu_core);
    void ReportUtil();

    bool isValidProcessName(const std::string& process_name);
    void print_CPU_UTIL();

    void vmstat() const;
};