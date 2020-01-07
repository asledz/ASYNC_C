#include "future.h"

/**
 * @file future.c
 * @brief Future Implementation File
 */

typedef enum {
    ready       =  0,
    pending     = -1
} future_status_t;

/**
 *  @struct data_t
 *  @brief Struct to store data for callable.
 *  @var to         Future to return value.
 *  @var from       Future to take value.
 *  @var function   Function to run on the value
 */
typedef struct data {
    future_t* to;
    future_t* from;
    void *(*function)(void *, size_t, size_t *);
} data_t;

typedef void *(*function_t)(void *);

void map_fun(void *args, size_t size);

void fun(void *future, size_t size);

/***************************************************
 ****************** MAIN FUNCTIONS *****************
 ***************************************************/

int async(thread_pool_t *pool, future_t *future, callable_t callable) {
    if(!pool || !future) {
        return invalid_argument_error;
    }

    if(!(future->callable = malloc(sizeof(callable_t)))) {
        return memory_error;
    }

    future->callable->function = callable.function;
    future->callable->arg = callable.arg;
    future->callable->argsz = callable.argsz;

    if(sem_init(&(future->mutex), THREAD_SEMAPHORE, 0)) {
        free(future->callable);
        return semaphore_error;
    }

    future->status = pending;

    int err;

    if((err = defer(pool, (runnable_t){
            .function = fun,
            .arg = future,
            .argsz = sizeof(future_t)}))) {
        free(future->callable);
        return err;
    }

    return success;
}


int map(thread_pool_t *pool, future_t *future, future_t *from,
        void *(*function)(void *, size_t, size_t *)) {

    if(!pool || !future || !from || !function) {
        return invalid_argument_error;
    }

    data_t *my_data = malloc(sizeof(data_t));
    my_data->to = future;
    my_data->from = from;
    my_data->function = function;

    if(sem_init(&(future->mutex), THREAD_SEMAPHORE, 0)) {
        free(my_data);
    }

    int err;

    if((err = defer(pool, (runnable_t){
            .function = map_fun,
            .arg = my_data,
            .argsz = sizeof(data_t)}))) {
        return err;
    }

    future->status = pending;

    return 0;
}

void *await(future_t *future) {
    if(sem_wait(&(future->mutex))) return (void*)semaphore_error;
    sem_destroy(&(future->mutex));
    return future->result;
}

/***************************************************
 ****************** UTIL FUNCTIONS *****************
 ***************************************************/

void fun(void *temp_future, size_t size  __attribute__((unused))){
    future_t *future = (future_t*)temp_future;
    future->result = (*(future->callable->function))(future->callable->arg, future->callable->argsz, &(future->result_size));

    future->status = ready;
    free(future->callable);
    if(sem_post(&(future->mutex))) return; //SEMAPHORE_ERROR
}

void map_fun(void *args, size_t size  __attribute__((unused))) {
    data_t *my_data = args;

    void* temp_res = await(my_data->from);
    size_t temp_size = my_data->from->result_size;

    my_data->to->status = ready;
    my_data->to->result = (my_data->function)(temp_res, temp_size, &(my_data->from->result_size));

    if(sem_post(&(my_data->to->mutex))) {
        exit(semaphore_error);
    }
    free(my_data);
}


