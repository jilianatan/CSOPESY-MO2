# CSOPESY Multitasking OS
This project implements a Multitasking Operating System with features focusing on memory management, process scheduling, and command-line interface (CLI) functionalities. It is designed to simulate key OS behaviors like memory allocation, process scheduling, and system-level commands. The system supports First-Come-First-Serve (FCFS) and Round-Robin (RR) schedulers while integrating memory visualization.

## Members (S11, Group 5)
- Ang, Mark Kevin 
- Latosa, Jose Romulo
- Tan, Jiliana Amibelle

## Getting Started
1. Download or clone the repository to your local machine.
2. Compile the program using your preferred C++ compiler.
3. Run the application to begin using the OS Emulator.

## Entry Class
The main entry point for the application is located in the Main.cpp file, where the main function is defined.

## Project Structure
### Relevant Files
- Main.cpp: Entry class where the main function is located.
- Commands.cpp / Commands.h: Manages command processing for the CLI.
- Console.cpp / Console.h: Handles console output and input operations.
- ConsoleManager.cpp / ConsoleManager.h: Oversees console-specific functionalities.
- FCFSScheduler.cpp / FCFSScheduler.h: Implements the First-Come, First-Served (FCFS) scheduling algorithm.
- MemoryManager.cpp / MemoryManager.h:  Base class for memory allocation and handles memory operations.
- RRScheduler.cpp / RRScheduler.h: Implements the Round Robin (RR) scheduling algorithm.
- Process.cpp / Process.h: Defines and manages processes within the scheduler.
- Thread.cpp / Thread.h: Provides threading utilities.
- Config.cpp / Config.h: Manages configuration settings, such as loading parameters from config.txt.
### Configuration File
- config.txt: Contains configuration settings for the emulator.




