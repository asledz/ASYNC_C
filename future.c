#include "future.h"

#define READY 1
#define PENDING 0

typedef void *(*function_t)(void *);

// util functions
typedef struct data {
    future_t* to;
    future_t* from;
    void *(*function)(void *, size_t, size_t *) ;//fixme check
} data;

void fun(void *future, size_t size){
    future_t *fut = (future_t*)future;
    fut->result = (*(fut->callable->function))(fut->callable->arg, fut->callable->argsz, &(fut->result_size));

    fut->status = READY;
    free(fut->callable);
    if(sem_post(&(fut->mutex))) return; //SEMAPHORE_ERROR
}

void map_fun(void *args, size_t size) {
    data *my_data = args;

    void* temp_res = await(my_data->from);
    size_t temp_size = my_data->from->result_size;

    my_data->to->result = (my_data->function)(temp_res, temp_size, &(my_data->from->result_size));

    my_data->to->status = READY;
//    free(my_data->from->callable);
    if(sem_post(&(my_data->to->mutex))) return; //todo: SEMAPHORE_ERROR
    free(my_data);
}

int async(thread_pool_t *pool, future_t *future, callable_t callable) {
    if(!pool || !future) {
        return invalid_argument_error;
    }

    // fixme: czy trzeba mallocowac callable???
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

    future->status = PENDING;

    int err;

    if((err = defer(pool, (runnable_t){
            .function = fun,
            .arg = future,
            .argsz = sizeof(future_t)}))) {
        return err;
    }

    return success;
}


int map(thread_pool_t *pool, future_t *future, future_t *from,
        void *(*function)(void *, size_t, size_t *)) {

    if(!pool || !future || !from || !function) {
        return invalid_argument_error;
    }

    data *mydata = malloc(sizeof(data));
    mydata->to = future;
    mydata->from = from;
    mydata->function = function;

    if(sem_init(&(future->mutex), THREAD_SEMAPHORE, 0)) {
        free(mydata);
    }

    int err;

    if((err = defer(pool, (runnable_t){
            .function = map_fun,
            .arg = mydata,
            .argsz = sizeof(data)}))) {
        return err;
    }

    future->status = PENDING;

    return 0;
}

void *await(future_t *future) {
    if(sem_wait(&(future->mutex))) return (void*)semaphore_error;
    sem_destroy(&(future->mutex));
    return future->result;
}

