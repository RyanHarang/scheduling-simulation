#include "Queue.h"

Queue *initialize_queue()
{
    Queue *queue = malloc(sizeof(Queue));
    if (queue == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

void enqueue(Queue *queue, Process *process)
{
    Node *new_node = malloc(sizeof(Node));
    if (!new_node)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    new_node->process = *process;
    new_node->next = NULL;

    if (queue->rear == NULL)
    {
        queue->front = new_node;
    }
    else
    {
        queue->rear->next = new_node;
    }

    queue->rear = new_node;
    queue->size++;
}

Process *dequeue(Queue *queue)
{
    if (is_empty(queue))
    {
        printf("Queue is empty\n");
        return NULL;
    }

    Node *temp = queue->front;
    Process *process = &(temp->process);
    queue->front = queue->front->next;
    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    queue->size--;
    return process;
}

int is_empty(Queue *queue)
{
    return queue->size == 0;
}

void free_queue(Queue *queue)
{
    Node *current = queue->front;
    while (current != NULL)
    {
        Node *next = current->next;
        free(current);
        current = next;
    }
    free(queue);
}