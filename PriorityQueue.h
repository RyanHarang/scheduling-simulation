#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "Queue.h"

// Structs
typedef struct
{
    Queue *queue;
} PriorityQueue;

// Function prototypes
void enqueue_priority(PriorityQueue *queue, Process *process);
Process *dequeue_priority(PriorityQueue *queue);
int is_empty_priority(PriorityQueue *queue);
PriorityQueue *initialize_priority_queue();
void free_priority_queue(PriorityQueue *priority_queue);

#endif
