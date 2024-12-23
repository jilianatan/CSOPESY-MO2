#pragma once
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Process.h"
#include <list>

class RR_Scheduler {

private:
    int num_cores;
    int time_quantum;
    bool running;
    std::vector<std::thread> cpu_threads;
    std::queue<Process*> process_queue;
   // std::mutex mtx;
    std::condition_variable cv;
    std::chrono::steady_clock::time_point start_time;
    std::list<Process*> running_processes;
    std::list<Process*> finished_processes;
    void cpu_worker(int core_id);
    mutable std::mutex mtx;

    size_t total_memory;
    size_t used_memory;
    size_t free_memory;


public:
    RR_Scheduler(int cores, int quantum, size_t total_memory);
    ~RR_Scheduler();

    size_t getIdleTicks() const;
    size_t getActiveTicks() const;
    size_t getTotalTicks() const;


    void add_process(Process* proc);
    void start();
    void stop();
    void print_running_processes();
    void print_finished_processes();
    void print_process_details(const std::string& process_name, int screen);
    void screen_ls();
    void print_process_queue_names();
    void SetCpuCore(int cpu_core);
    void SetQuantum(int quantum);
    bool isValidProcessName(const std::string& process_name);
    void ReportUtil();
    float GetCpuUtilization();
    void print_CPU_UTIL();

    void vmstat() const;
};
