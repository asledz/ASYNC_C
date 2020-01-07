#ifndef FUTURE_H
#define FUTURE_H

#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "threadpool.h"

/**
 * @file future.h
 * @brief Future Header File
 */

typedef struct callable {
  void *(*function)(void *, size_t, size_t *);
  void *arg;
  size_t argsz;
} callable_t;

/**
 *  @struct future_t
 *  @brief The future struct.
 *  @var *callable              Function to return the value from.
 *  @var *result                Result value of the callable.
 *  @var result_size            Size of the result value.
 *  @var status                 Status of the future(pending/ready).
 *  @var mutex                  Mutex of future.
 */
typedef struct future {
    callable_t *callable;
    void *result;
    size_t result_size;
    bool status;
    sem_t mutex;
} future_t;

/**
 * @function async
 * @brief Assigns callable future and adds callable to the threadpool.
 * @param pool          Where to add callable.
 * @param future        Future to assign.
 * @param callable      Callable to run.
 * @return 0 if all goes correctly, negative values in case of an error(@see thread_pool_error_t for codes).
 */
int async(thread_pool_t *pool, future_t *future, callable_t callable);

/**
 * @function map
 * @brief Maps the value returned by one future to another by given function.
 * @param pool          Pool to operate on.
 * @param future        Future to return the value.
 * @param from          Value to map the result.
 * @param *function     How to map the value.
 * @return 0 if all goes correctly, negative values in case of an error(@see thread_pool_error_t for codes).
 */
int map(thread_pool_t *pool, future_t *future, future_t *from,
        void *(*function)(void *, size_t, size_t *));

/**
 * @function await
 * @brief Waits and returns the value counted by future.
 * @param future        Value to return.
 * @return value.
 */
void *await(future_t *future);

#endif
