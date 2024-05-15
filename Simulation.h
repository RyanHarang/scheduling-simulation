#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "PriorityQueue.h"

// Global variables
extern char *inputFileName;         // Name of input file
extern int quantumA;                // Quantum for queue A
extern int quantumB;                // Quantum for queue B
extern int preemption;              // 1reemption enabled, 0reemption disabled
extern int current_time;            // Current time
extern Queue *queueA;               // Queue A
extern PriorityQueue *queueB;       // Queue B
extern ProcessList *process_list;   // List of processes
extern ProcessList *active_io_list; // List of processes in io

// Global variables for analysis
extern int start_time;
extern int end_time;
extern int total_ready_time;
extern int max_ready_time;
extern int min_ready_time;
extern int num_processes_completed;
extern int num_instructions_completed;
extern ProcessList *completed_processes;

// Function prototypes
void run_simulation();
void parse_input_file(char *filename);
void ensure_instruction_memory(Process *process);
void initialize_queues();
ProcessList *initialize_process_list();
void add_process_to_list(ProcessList *list, Process *process);
void remove_process_from_list(ProcessList *list, int index);
void free_process_list(ProcessList *list);
void schedule_processes();
void check_new_arrivals();
bool increment_current_time(Process *current_process);
void execute_instruction(Process *process, bool preempting);
void execute_io_instructions();
void preempt_process(Process *process, Process *new_process);
void terminate_process(Process *process);
void check_for_promotion(Process *process);
void print_results();

#endif