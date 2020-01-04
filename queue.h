//
// Created by Anita on 02/01/2020.
// source: https://codereview.stackexchange.com/questions/141238/implementing-a-generic-queue-in-c

#ifndef ASYNC_QUEUE_H
#define ASYNC_QUEUE_H

#include <stddef.h>

typedef struct Node
{
    void *data;
    struct Node *next;
}node;

typedef struct QueueList
{
    int sizeOfQueue;
    size_t memSize;
    node *head;
    node *tail;
}Queue;

void queueInit(Queue *q, size_t memSize);
int enqueue(Queue *, void *);
void* dequeue(Queue *);
int getQueueSize(Queue *);

#endif //ASYNC_QUEUE_H
