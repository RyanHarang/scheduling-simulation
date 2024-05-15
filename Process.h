#ifndef PROCESS_H
#define PROCESS_H

#include "Constants.h"

// Structs
typedef struct
{
    char type; // 'a' = arrival, 'i' = io, 'e' = exe, 't' = terminate
    int time;  // Time of instruction
} Instruction;

typedef struct
{
    int id;                    // Process ID
    int priority;              // Priority
    int arrival_time;          // Time process arrived
    int total_run_time;        // Total time process runs
    int quantum_remaining;     // Time remaining in quantum
    int in_queue_a;            // 1 if in queue A, 0 otherwise
    int end_time;              // Time process ended
    int wait_time;             // Time spent waiting
    int successive_bursts;     // Successive bursts with remaining quantum
    Instruction *instructions; // Array of instructions
    int current_instruction;   // Index of current instruction
    int instructions_count;    // Number of instructions
    int instructions_capacity; // Capacity of array
} Process;

typedef struct
{
    Process **processes; // Array of processes
    int size;            // Number of processes
    int index;           // Index of next process
    int capacity;        // Capacity of array
} ProcessList;

#endif