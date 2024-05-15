#include "PriorityQueue.h"

PriorityQueue *initialize_priority_queue()
{
    PriorityQueue *priority_queue = malloc(sizeof(PriorityQueue));
    if (priority_queue == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    priority_queue->queue = initialize_queue();
    return priority_queue;
}

int is_empty_priority(PriorityQueue *priority_queue)
{
    return is_empty(priority_queue->queue);
}

void enqueue_priority(PriorityQueue *priority_queue, Process *process)
{
    if (is_empty_priority(priority_queue))
    {
        enqueue(priority_queue->queue, process);
    }
    else
    {
        Node *new_node = malloc(sizeof(Node));
        if (new_node == NULL)
        {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }

        new_node->process = *process;
        new_node->next = NULL;

        Node *current = priority_queue->queue->front;
        Node *prev = NULL;
        while (current != NULL && current->process.priority > process->priority)
        {
            prev = current;
            current = current->next;
        }

        if (prev == NULL)
        {
            new_node->next = priority_queue->queue->front;
            priority_queue->queue->front = new_node;
        }
        else
        {
            prev->next = new_node;
            new_node->next = current;
        }

        if (current == NULL)
        {
            priority_queue->queue->rear = new_node;
        }

        priority_queue->queue->size++;
    }
}

Process *dequeue_priority(PriorityQueue *priority_queue)
{
    return dequeue(priority_queue->queue);
}

void free_priority_queue(PriorityQueue *priority_queue)
{
    free_queue(priority_queue->queue);
    free(priority_queue);
}