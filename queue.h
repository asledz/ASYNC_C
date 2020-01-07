// code based on: https://codereview.stackexchange.com/questions/141238/implementing-a-generic-queue-in-c
// generic queue implementation.

#ifndef ASYNC_QUEUE_H
#define ASYNC_QUEUE_H

#include <stddef.h>

/**
 * @file queue.h
 * @brief Queue header file
 */

typedef struct Node {
    void *data;
    struct Node *next;
} node;

typedef struct QueueList {
    int sizeOfQueue;
    node *head;
    node *tail;
} Queue;

void queueInit(Queue *q);

int enqueue(Queue *, void *);

void *dequeue(Queue *);

int getQueueSize(Queue *);

#endif //ASYNC_QUEUE_H
