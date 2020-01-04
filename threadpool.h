#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"


typedef enum {
    success                 = 0,
    invalid_argument_error  = -1,
    memory_error            = -2,
    pthread_error           = -3,
    semaphore_error         = -4,
    queue_error             = -5,
    user_error              = -6

} thread_pool_error_t;

#define THREAD_SEMAPHORE 0


typedef struct runnable {
    void (*function)(void *, size_t);
    void *arg;
    size_t argsz;
} runnable_t;

typedef struct thread_pool {
    pthread_t *threads;
    size_t size;
    Queue *runnables; // task

    bool end;
    pthread_attr_t attr;

    sem_t runnables_semaphore;
    sem_t pool_mutex;

} thread_pool_t;

int thread_pool_init(thread_pool_t *pool, size_t pool_size);

void thread_pool_destroy(thread_pool_t *pool);

int defer(thread_pool_t *pool, runnable_t runnable);

#endif
