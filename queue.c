//
// Created by Anita on 02/01/2020.
// source: https://codereview.stackexchange.com/questions/141238/implementing-a-generic-queue-in-c

#include "queue.h"


#include <stdlib.h>

void queueInit(Queue *q, size_t memSize)
{
    q->sizeOfQueue = 0;
    q->memSize = memSize;
    q->head = q->tail = NULL;
}
int enqueue(Queue *q, void *data) {
    node *newNode = (node *) malloc(sizeof(node));

    if (newNode == NULL) {
        return -1;
    }

    newNode->data = data;

    newNode->next = NULL;

    if (q->sizeOfQueue == 0) {
        q->head = q->tail = newNode;
    } else {
        q->tail->next = newNode;
        q->tail = newNode;
    }

    q->sizeOfQueue++;
    return 0;
}

void* dequeue(Queue *q)
{
    if(q->sizeOfQueue > 0)
    {
        node *temp = q->head;

        if(q->sizeOfQueue > 1)
        {
            q->head = q->head->next;
        }
        else
        {
            q->head = NULL;
            q->tail = NULL;
        }

        q->sizeOfQueue--;
        void *data = temp->data;
//        free(temp->data);
        free(temp);
        return data;
    }
}


int getQueueSize(Queue *q)
{
    return q->sizeOfQueue;
}