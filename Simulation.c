#include "Simulation.h"

// Global variables
char *inputFileName;
int quantumA;
int quantumB;
int preemption;
int current_time = 0;
Queue *queueA;
PriorityQueue *queueB;
ProcessList *process_list;
ProcessList *active_io_list;

// Global variables for analysis
int start_time = 0;
int end_time = -1;
int max_ready_time = 0;
int min_ready_time = -1;
int num_processes_completed = 0;
int num_instructions_completed = 0;
ProcessList *completed_processes;

// Function Prototypes
void run_simulation();
void parse_input_file(char *filename);
void schedule_processes();
void check_new_arrivals();
bool increment_current_time(Process *current_process);
void execute_instruction(Process *process, bool preempting);
void execute_io_instructions();
void preempt_process(Process *process, Process *new_process);
void terminate_process(Process *process);
void check_for_promotion(Process *process);
void print_results();

// External function prototypes
extern void ensure_instruction_memory(Process *process);
extern void initialize_queues();
extern ProcessList *initialize_process_list();
extern void add_process_to_list(ProcessList *list, Process *process);
extern void remove_process_from_list(ProcessList *list, int index);
extern void free_process_list(ProcessList *list);

int main(int argc, char *argv[])
{
    // Check number of arguments
    if (argc != 5)
    {
        printf("Incorrect num of arguments\n");
        return 1;
    }

    // Parse arguments
    inputFileName = argv[1];
    quantumA = atoi(argv[2]);
    quantumB = atoi(argv[3]);
    preemption = atoi(argv[4]);

    run_simulation();

    return 0;
}

void run_simulation()
{
    process_list = initialize_process_list();
    active_io_list = initialize_process_list();
    completed_processes = initialize_process_list();
    initialize_queues();
    parse_input_file(inputFileName);
    schedule_processes();
    print_results();
    free_process_list(process_list);
    free_process_list(active_io_list);
    free_process_list(completed_processes);
    free_queue(queueA);
    free_priority_queue(queueB);
}

void parse_input_file(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Error opening file\n");
        return;
    }

    char line[256];
    Process *current_process = NULL;

    while (fgets(line, sizeof(line), file))
    {
        // Check if process line
        if (line[0] == 'P')
        {

            // Extract process ID and priority
            int process_id, priority;
            sscanf(line, "P%d:%d", &process_id, &priority);

            // Create new process
            current_process = malloc(sizeof(Process));
            if (current_process == NULL)
            {
                fprintf(stderr, "Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
            *current_process = (Process){0};
            current_process->id = process_id;
            current_process->priority = priority;
        }
        // Check if arrival line
        else if (line[0] == 'a')
        {
            // Extract arrival time
            int arrival_time;
            sscanf(line, "arrival_t:%d", &arrival_time);
            current_process->arrival_time = arrival_time;
        }
        // Check if instruction line
        else if (line[0] == 'i' || line[0] == 'e' || line[0] == 't')
        {
            if (line[0] == 't')
            {
                char instruction_type = 't';
                int time = 1;

                ensure_instruction_memory(current_process);

                // Add instruction to process
                Instruction instruction = {0};
                instruction.type = instruction_type;
                instruction.time = time;
                current_process->instructions[current_process->instructions_count] = instruction;
                current_process->instructions_count++;
                add_process_to_list(process_list, current_process);
            }
            else
            {
                char *token = strtok(line, ":");
                if (token != NULL)
                {
                    char instruction_type = token[0];
                    int time;

                    token = strtok(NULL, ":");
                    if (token != NULL)
                    {
                        time = atoi(token);

                        ensure_instruction_memory(current_process);

                        // Add instruction to process
                        Instruction instruction = {0};
                        instruction.type = instruction_type;
                        instruction.time = time;
                        current_process->instructions[current_process->instructions_count] = instruction;
                        current_process->instructions_count++;
                    }
                    else
                    {
                        fprintf(stderr, "Error parsing instruction line: %s\n", line);
                    }
                }
                else
                {
                    fprintf(stderr, "Error parsing instruction line: %s\n", line);
                }
            }
        }
    }

    completed_processes->index = 0;
    active_io_list->index = 0;
    process_list->index = 0;
    start_time = process_list->processes[0]->arrival_time;
    fclose(file);
}

void schedule_processes()
{
    check_new_arrivals();

    // Check for processes in queues, process list, or io list
    while (!(is_empty(queueA) && is_empty_priority(queueB) && process_list->index >= process_list->size && active_io_list->size == 0))
    {
        if (is_empty(queueA))
        {
            // Execute from queueB
            if (!is_empty_priority(queueB))
            {
                Process *process = dequeue_priority(queueB);
                execute_instruction(process, false);
            }
            // Both queues empty, increment time
            else
            {
                increment_current_time(NULL);
            }
        }
        // Execute from queueA
        else
        {
            Process *process = dequeue(queueA);
            execute_instruction(process, false);
        }
    }
}

void check_new_arrivals()
{
    while (process_list->index < process_list->size)
    {
        if (process_list->processes[process_list->index]->arrival_time <= current_time)
        {
            Process *process = process_list->processes[process_list->index];
            process_list->index++;
            process->quantum_remaining = quantumB;
            enqueue_priority(queueB, process);
        }
        else
        {
            break;
        }
    }
}

bool increment_current_time(Process *current_process)
{
    current_time++;
    check_new_arrivals();
    bool preempted = false;
    // Preemption enabled, active process, queueB not empty
    if (preemption && current_process != NULL && !is_empty_priority(queueB))
    {
        // Current process from queueB
        if (current_process->in_queue_a == 0)
        {
            Process *new_process = dequeue_priority(queueB);

            // Head of queueB has greater priority
            if (new_process->priority > current_process->priority)
            {
                preempt_process(current_process, new_process);
                preempted = true;
            }
            else
            {
                enqueue_priority(queueB, new_process);
            }
        }
    }
    execute_io_instructions();
    return preempted;
}

void execute_instruction(Process *process, bool preempting)
{
    if (process == NULL || process->instructions == NULL)
    {
        fprintf(stderr, "Error: process or instructions are NULL\n");
        return;
    }

    if (process->current_instruction >= process->instructions_count)
    {
        fprintf(stderr, "Error: current_instruction out of bounds\n");
        return;
    }

    bool decoded = false;

    while (process->quantum_remaining > 0)
    {
        Instruction *current_instruction = &(process->instructions[process->current_instruction]);
        char instruction_type = current_instruction->type;
        // Handle decoding
        if (!decoded)
        {
            if (!preempting)
            {
                if (increment_current_time(process))
                {
                    return;
                }
            }
            process->quantum_remaining--;
            process->total_run_time++;
            decoded = true;
            // Handle terminate instruction
            if (instruction_type == 't')
            {
                current_instruction->time--;
                terminate_process(process);
                return;
            }
            // Handle io instruction
            else if (instruction_type == 'i')
            {
                if (process->quantum_remaining > 0)
                {
                    check_for_promotion(process);
                }
                else
                {
                    process->successive_bursts = 0;
                }
                add_process_to_list(active_io_list, process);
                return;
            }
            else if (process->quantum_remaining == 0)
            {
                // Reset quantum, bursts, return process to queue
                process->successive_bursts = 0;
                process->in_queue_a ? (process->quantum_remaining = quantumA) : (process->quantum_remaining = quantumB);
                process->in_queue_a ? enqueue(queueA, process) : enqueue_priority(queueB, process);
                return;
            }
            preempting = false;
        }
        // Handle exe instruction
        else
        {
            if (!preempting)
            {
                if (increment_current_time(process))
                {
                    return;
                }
            }
            current_instruction->time--;
            process->quantum_remaining--;
            process->total_run_time++;

            // Quantum finishes
            if (process->quantum_remaining == 0)
            {
                process->successive_bursts = 0;

                // And instruction finishes
                if (current_instruction->time == 0)
                {
                    // Move to next instruction
                    process->current_instruction++;
                }
                // Reset quantum, return process to queue
                process->in_queue_a ? (process->quantum_remaining = quantumA) : (process->quantum_remaining = quantumB);
                process->in_queue_a ? enqueue(queueA, process) : enqueue_priority(queueB, process);
                return;
            }

            // Only instruction finishes
            else if (current_instruction->time == 0)
            {
                // Move to next instruction, reset decoded
                process->current_instruction++;
                decoded = false;
            }
            preempting = false;
        }
    }
}

void execute_io_instructions()
{
    for (int i = 0; i < active_io_list->size; i++)
    {
        Process *process = active_io_list->processes[i];
        Instruction *current_instruction = &(process->instructions[process->current_instruction]);
        current_instruction->time--;
        process->total_run_time++;

        // Check if process has finished io instruction
        if (current_instruction->time == 0)
        {
            // Move to next instruction
            process->current_instruction++;

            // Reset quantum, return process to queue, remove process from active_io_list
            process->in_queue_a ? (process->quantum_remaining = quantumA) : (process->quantum_remaining = quantumB);
            process->in_queue_a ? enqueue(queueA, process) : enqueue_priority(queueB, process);
            remove_process_from_list(active_io_list, i);
        }
    }
}

void preempt_process(Process *process, Process *new_process)
{
    if (process->quantum_remaining > 0)
    {
        check_for_promotion(process);
    }
    process->in_queue_a ? (process->quantum_remaining = quantumA) : (process->quantum_remaining = quantumB);
    process->in_queue_a ? enqueue(queueA, process) : enqueue_priority(queueB, process);
    execute_instruction(new_process, true);
}

void terminate_process(Process *process)
{
    num_processes_completed++;
    add_process_to_list(completed_processes, process);
    num_instructions_completed += (process->instructions_count);
    process->end_time = current_time;
    process->wait_time = (process->end_time - process->total_run_time - process->arrival_time);
}

void check_for_promotion(Process *process)
{
    process->successive_bursts++;
    if (process->successive_bursts >= 3)
    {
        process->in_queue_a = 1;
    }
}

void print_results()
{
    float total_wait_time = 0;
    for (int i = 0; i < num_processes_completed; i++)
    {
        int wait_time = completed_processes->processes[i]->wait_time;
        total_wait_time += wait_time;
        if (wait_time > max_ready_time)
        {
            max_ready_time = wait_time;
        }
        if (min_ready_time == -1 || wait_time < min_ready_time)
        {
            min_ready_time = wait_time;
        }
    }
    float average_ready_time = total_wait_time / num_processes_completed;

    printf("Start/end time: %d, %d\n", start_time, current_time);
    printf("Processes Completed: %d\n", num_processes_completed);
    printf("Instructions Completed: %d\n", num_instructions_completed);
    printf("Ready time average: %.2f\n", average_ready_time);
    printf("Ready time maximum: %d\n", max_ready_time);
    printf("Ready time minimum: %d\n", min_ready_time);

    for (int i = 0; i < num_processes_completed; i++)
    {
        Process *process = completed_processes->processes[i];
        printf("P%d time_completion:%d time_waiting:%d termination_queue:%c\n",
               process->id,
               process->end_time,
               process->wait_time,
               process->in_queue_a ? 'A' : 'B');
    }
}