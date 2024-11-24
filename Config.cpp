#include "Config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath> 

Config::ConfigParameters Config::config_parameters_;

void Config::Initialize() {
    std::ifstream config_file("config.txt");
    if (!config_file.is_open()) {
        std::cout << "Failed to open file \"config.txt\"" << std::endl;
        return;
    }

    std::string key_value;
    while (getline(config_file, key_value)) {
        // Read line.
        std::stringstream key_value_stream(key_value);
        std::string key, value;
        // Get parameter key and value.
        key_value_stream >> key >> value;
        value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());

        if (key == "num-cpu") {
            config_parameters_.num_cpu = std::stoi(value);
        }
        else if (key == "scheduler") {
            if (value == "fcfs" || value == "rr") {
                config_parameters_.scheduler = value;
            }
            else {
                std::cout << "Invalid scheduler value: " << value << std::endl;
            }
        }
        else if (key == "quantum-cycles") {
            config_parameters_.quantum_cycles = std::stoi(value);
        }
        else if (key == "batch-process-freq") {
            config_parameters_.batch_process_freq = std::stod(value);
        }
        else if (key == "min-ins") {
            config_parameters_.min_ins = std::stoi(value);
        }
        else if (key == "max-ins") {
            config_parameters_.max_ins = std::stoi(value);
        }
        else if (key == "delay-per-exec") {
            config_parameters_.delay_per_exec = std::stod(value);
        }
        else if (key == "max-overall-mem") {
            config_parameters_.max_overall_mem = std::stoul(value);
        }
        else if (key == "mem-per-frame") {
            config_parameters_.mem_per_frame = std::stoul(value);
        }
        else if (key == "min-mem-per-proc") {
            config_parameters_.min_mem_per_proc = std::stoul(value);
        }
        else if (key == "max-mem-per-proc") {
            config_parameters_.max_mem_per_proc = std::stoul(value);
        }
        else {
            std::cout << "Invalid config key: " << key << std::endl;
        }
    }


    if (config_parameters_.max_overall_mem == 0 || config_parameters_.mem_per_frame == 0) {
        std::cerr << "Invalid configuration: max-overall-mem and mem-per-frame must be non-zero." << std::endl;
        std::exit(EXIT_FAILURE); // Exit the program if critical parameters are invalid
    }

    if (config_parameters_.max_overall_mem % config_parameters_.mem_per_frame != 0) {
        std::cerr << "Invalid configuration: max-overall-mem must be a multiple of mem-per-frame." << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

Config::ConfigParameters Config::GetConfigParameters() {
    return config_parameters_;
}
