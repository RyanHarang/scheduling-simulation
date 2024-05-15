#include <stdio.h>
#include <stdlib.h>
#include "Simulation.h"

// Function Prototypes
void ensure_instruction_memory(Process *process);
void initialize_queues();
ProcessList *initialize_process_list();
void add_process_to_list(ProcessList *list, Process *process);
void remove_process_from_list(ProcessList *list, int index);
void free_process_list(ProcessList *list);
void update_queues_wait_time();

void ensure_instruction_memory(Process *process)
{
    if (process->instructions == NULL)
    {
        process->instructions = malloc(sizeof(Instruction) * INITIAL_INSTRUCTION_CAPACITY);
        if (process->instructions == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            return;
        }
        process->instructions_capacity = INITIAL_INSTRUCTION_CAPACITY;
    }
    else if (process->instructions_count >= process->instructions_capacity)
    {
        int new_capacity = process->instructions_capacity * 2;
        Instruction *new_instructions = realloc(process->instructions, sizeof(Instruction) * new_capacity);
        if (new_instructions == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            return;
        }
        process->instructions = new_instructions;
        process->instructions_capacity = new_capacity;
    }
}

void initialize_queues()
{
    queueA = initialize_queue();
    queueB = initialize_priority_queue();
}

ProcessList *initialize_process_list()
{
    ProcessList *list = malloc(sizeof(ProcessList));
    if (list == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    list->processes = malloc(sizeof(Process *) * INITIAL_PROCESS_CAPACITY);
    if (list->processes == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        free(list);
        return NULL;
    }
    list->size = 0;
    list->capacity = INITIAL_PROCESS_CAPACITY;
    return list;
}

void add_process_to_list(ProcessList *list, Process *process)
{
    if (list->size >= list->capacity)
    {
        // Double capacity of array
        int new_capacity = list->capacity * 2;
        Process **new_processes = realloc(list->processes, sizeof(Process *) * new_capacity);
        if (new_processes == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            return;
        }
        list->processes = new_processes;
        list->capacity = new_capacity;
    }
    list->processes[list->size++] = process;
}

void remove_process_from_list(ProcessList *list, int index)
{
    if (index >= 0 && index < list->size)
    {
        // free(list->processes[index]);

        for (int i = index; i < list->size - 1; i++)
        {
            list->processes[i] = list->processes[i + 1];
        }

        list->size--;
    }
    else
    {
        fprintf(stderr, "Invalid index\n");
    }
}

void free_process_list(ProcessList *list)
{
    if (list != NULL)
    {
        free(list->processes);
        free(list);
    }
}