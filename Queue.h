#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include "Process.h"

// Structs
typedef struct Node
{
    Process process;
    struct Node *next;
} Node;

typedef struct
{
    Node *front; // Front of the queue
    Node *rear;  // Rear of the queue
    int size;    // Number of elements in the queue
} Queue;

// Function prototypes
void enqueue(Queue *queue, Process *process);
Process *dequeue(Queue *queue);
int is_empty(Queue *queue);
Queue *initialize_queue();
void free_queue(Queue *queue);

#endif
