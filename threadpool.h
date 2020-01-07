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

/**
 * @file threadpool.h
 * @brief Threadpool Header File
 */

typedef enum {
    success                 =  0,
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

/**
 *  @struct thread_pool_t
 *  @brief The threadpool struct
 *  @var *threads               Array of all of the threads.
 *  @var size                   Amount of threads.
 *  @var *runnables             Queue of the tasks.
 *  @var end                    Flag, that forbids to add new tasks after destroy is called.
 *  @var attr                   Initializer for threads.
 *  @var runnables_semaphore    Semaphore for the queue.
 *  @var pool_mutex             Semaphore for the pool
 */
typedef struct thread_pool {
    pthread_t *threads;
    size_t size;

    Queue *runnables;
    bool end;
    pthread_attr_t attr;

    sem_t runnables_semaphore;
    sem_t pool_mutex;

} thread_pool_t;

/**
 * @function thread_pool_init
 * @brief Creates a thread_pool_t object at argument pool.
 * @param pool          Where to initialize threadpool.
 * @param pool_size     Number of threads.
 * @return 0 if all goes correctly, negative values in case of an error(@see thread_pool_error_t for codes).
 */
int thread_pool_init(thread_pool_t *pool, size_t pool_size);

/**
 * @function thread_pool_destroy
 * @brief Finishes all of the tasks, stops and removes pool.
 * @param pool          Which threadpool to destroy.
 */
void thread_pool_destroy(thread_pool_t *pool);

/**
 * @function defer
 * @brief Adds a new task to the pool.
 * @param pool          Thread pool to execute the task.
 * @param runnable      Task to run on the pool.
 * @return 0 if all goes correctly, negative values in case of an error(@see thread_pool_error_t for codes).
 */
int defer(thread_pool_t *pool, runnable_t runnable);

#endif
