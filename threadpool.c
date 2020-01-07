#include "threadpool.h"

/**
 * @file threadpool.c
 * @brief Threadpool implementation file
 */

#define THREAD_SEMAPHORE 0

/***************************************************
 ****************** MAIN FUNCTIONS *****************
 ***************************************************/

static void *thread_pool_thread(void *pool);

int thread_pool_init(thread_pool_t *pool, size_t num_threads) {
    /* Checks if the pointer is valid. */
    if (!pool) {
        return invalid_argument_error;
    }

    /* Initializes the pool. */
    pool->end = false;
    pool->size = 0;

    /* Inits the attr. */
    if((pthread_attr_init(&(pool->attr))) < 0) {
        return pthread_error;
    }

    /* Initializes semaphores. */
    if((sem_init(&(pool->runnables_semaphore), THREAD_SEMAPHORE, 0))) {
        pthread_attr_destroy(&(pool->attr));
        return semaphore_error;
    }
    if((sem_init(&(pool->pool_mutex), THREAD_SEMAPHORE, 1))) {
        pthread_attr_destroy(&(pool->attr));
        sem_destroy(&(pool->runnables_semaphore));
        return semaphore_error;
    }

    /* Allocates memory and initializes the Queue. */
    if(!(pool->runnables = malloc(sizeof(Queue)))) {
        pthread_attr_destroy(&(pool->attr));
        sem_destroy(&(pool->runnables_semaphore));
        sem_destroy(&(pool->pool_mutex));
        return memory_error;
    }
    queueInit(pool->runnables);

    /* Allocates memory and start worker threads. */
    if(!(pool->threads = malloc(sizeof(pthread_t) * num_threads))) {
        pthread_attr_destroy(&(pool->attr));
        sem_destroy(&(pool->runnables_semaphore));
        sem_destroy(&(pool->pool_mutex));
        free(pool->runnables);
        return memory_error;
    }
    while(pool->size != num_threads) {
        if(pthread_create(&(pool->threads[pool->size]), &(pool->attr), thread_pool_thread, (void*)pool)) {
            thread_pool_destroy(pool);
            return pthread_error;
        }
        pool->size++;
    }

    return success;
}


void thread_pool_destroy(struct thread_pool *pool) {
    /* Checks if the pointer is valid. */
    if(pool == NULL) {
        return;
    }
    if(sem_wait(&(pool->pool_mutex))){
        exit(semaphore_error);
    }

    /* Sets the flag for the threads. */
    pool->end = true;
    if(sem_post(&(pool->pool_mutex))) {
        exit(semaphore_error);
    }
    for(size_t i = 0; i < pool->size; i++) {
        if(sem_post(&(pool->runnables_semaphore))){
            exit(semaphore_error);
        }
    }

    void *return_value;
    for(size_t i = 0; i < pool->size; i++) {
        /* Waits, until all of the tasks are finished. */
        (pthread_join((pool->threads[i]), &return_value));
    }
    /* Destroys the pool */
    pthread_attr_destroy(&(pool->attr));
    sem_destroy(&(pool->runnables_semaphore));
    sem_destroy(&(pool->pool_mutex));
    free(pool->runnables);
    free(pool->threads);
}

int defer(struct thread_pool *pool, runnable_t runnable) {
    /* Checks if the pointer is valid. */
    if(pool == NULL) {
        return invalid_argument_error;
    }

    /* Gets permission to edit pool. */
    if(sem_wait(&(pool->pool_mutex))){
        return semaphore_error;
    }

    /* Checks, if is allowed do add new task to the pool. */
    if(pool->end == true){
        if(sem_post(&(pool->pool_mutex))) {
            return semaphore_error;
        }
        return user_error;
    }

    /* Adds the task. */
    runnable_t* new_runnable;
    if(!(new_runnable = malloc(sizeof(runnable)))) {
        return memory_error;
    }

    new_runnable->function = runnable.function;
    new_runnable->arg = runnable.arg;
    new_runnable->argsz = runnable.argsz;

    if(enqueue(pool->runnables, new_runnable) || (sem_post(&(pool->runnables_semaphore)))) {
        free(new_runnable);
        return queue_error;
    }

    if(sem_post(&(pool->pool_mutex))) {
        free(new_runnable);
        return semaphore_error;
    }

    return success;
}

/***************************************************
 ****************** UTIL FUNCTIONS *****************
 ***************************************************/

// Worker
static void *thread_pool_thread(void *threadpool) {
   thread_pool_t *pool = (thread_pool_t*) threadpool;

   while(true){
       /* Waits for the task. */
       if(sem_wait(&(pool->runnables_semaphore))) {
           exit(semaphore_error);
       }

       /* Waits for permission to get the pool, when has task. */
       if(sem_wait(&(pool->pool_mutex))) {
           exit(semaphore_error);
       }

       /* Checks, if has to stop. */
       if(pool->end && getQueueSize(pool->runnables) == 0) {
           if(sem_post(&(pool->pool_mutex))){
               exit(semaphore_error);
           }
           break;
       }

       /* Deletes the task from queue. */
       runnable_t *task = dequeue(pool->runnables);

       /* Returns semaphore. */
       if(sem_post(&(pool->pool_mutex))) {
           exit(semaphore_error);
       }

       /* Run the task. */
       (*(task->function))(task->arg, task->argsz);

       free(task);
   }
}