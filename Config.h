#pragma once

#include <string>

class Config {
public:
	struct ConfigParameters {
		int num_cpu;
		std::string scheduler;
		int quantum_cycles;
		double batch_process_freq;
		int min_ins;
		int max_ins;
		double delay_per_exec;

		// New parameters for memory management
		size_t max_overall_mem = 0;  // Maximum memory available (in KB)
		size_t mem_per_frame = 0;    // Memory per frame (in KB)
		size_t min_mem_per_proc = 0; // Minimum memory per process (in KB)
		size_t max_mem_per_proc = 0; // Maximum memory per process (in KB)

	};
	static void Initialize();
	static ConfigParameters GetConfigParameters();

private:
	static ConfigParameters config_parameters_;
};