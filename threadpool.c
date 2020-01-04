#include "threadpool.h"

// For declaration of semaphore
#define THREAD_SEMAPHORE 0

static void *threadpool_thread(void *pool);

int thread_pool_init(thread_pool_t *pool, size_t num_threads) {
    if (!pool) {
        return invalid_argument_error;
    }

    pool->end = false;
    pool->size = 0;
    if((pthread_attr_init(&(pool->attr))) < 0) {
        return pthread_error;
    }

    if((sem_init(&(pool->runnables_semaphore), THREAD_SEMAPHORE, 0))) {
        pthread_attr_destroy(&(pool->attr));
        return semaphore_error;
    }
    if((sem_init(&(pool->pool_mutex), THREAD_SEMAPHORE, 1))) {
        pthread_attr_destroy(&(pool->attr));
        sem_destroy(&(pool->runnables_semaphore));
        return semaphore_error;
    }
    if(!(pool->runnables = malloc(sizeof(Queue)))) {
        pthread_attr_destroy(&(pool->attr));
        sem_destroy(&(pool->runnables_semaphore));
        sem_destroy(&(pool->pool_mutex));
        return memory_error;
    }
    queueInit(pool->runnables, sizeof(runnable_t));
    if(!(pool->threads = malloc(sizeof(pthread_t) * num_threads))) {
        pthread_attr_destroy(&(pool->attr));
        sem_destroy(&(pool->runnables_semaphore));
        sem_destroy(&(pool->pool_mutex));
        free(pool->runnables);
        return memory_error;
    }
    while(pool->size != num_threads) {
        if(pthread_create(&(pool->threads[pool->size]), &(pool->attr), threadpool_thread, (void*)pool)) {
            thread_pool_destroy(pool);
            return pthread_error;
        }
        pool->size++;
    }
    return success;
}


void thread_pool_destroy(struct thread_pool *pool) {
    if(pool == NULL) {
        return;
    }

    sem_wait(&(pool->pool_mutex));

    pool->end = true;

    if(sem_post(&(pool->pool_mutex))) return;
    for(size_t i = 0; i < pool->size; i++) {
        sem_post(&(pool->runnables_semaphore));
    }

    void *retval;
    for(size_t i = 0; i < pool->size; i++) {
        (pthread_join((pool->threads[i]), &retval));
    }

    pthread_attr_destroy(&(pool->attr));
    sem_destroy(&(pool->runnables_semaphore));
    sem_destroy(&(pool->pool_mutex));
    free(pool->runnables);
    free(pool->threads);

    // return SUCCESS;
}

int defer(struct thread_pool *pool, runnable_t runnable) {
    if(pool == NULL) {
        return invalid_argument_error;
    }
    while (sem_wait(&(pool->pool_mutex))) {
        return semaphore_error;
    }

    if(pool->end == true){
        if(sem_post(&(pool->pool_mutex))) return semaphore_error;
        return user_error;
    }

    runnable_t* new_runnable = malloc(sizeof(runnable));
    new_runnable->function = runnable.function;
    new_runnable->arg = runnable.arg;
    new_runnable->argsz = runnable.argsz;

    if(enqueue(pool->runnables, new_runnable)) return queue_error;

    if(sem_post(&(pool->runnables_semaphore))) return semaphore_error;

    if(sem_post(&(pool->pool_mutex))) return semaphore_error;

    return success;
}


static void *threadpool_thread(void *threadpool) {

   thread_pool_t *pool = (thread_pool_t*) threadpool;

   while(true){
       // FIXME
       if(sem_wait(&(pool->runnables_semaphore))) return semaphore_error;

       // FIXME
       if(sem_wait(&(pool->pool_mutex))) return semaphore_error;

       if(pool->end && getQueueSize(pool->runnables) == 0) {
           // FIXME
           if(sem_post(&(pool->pool_mutex))) return semaphore_error;
           break;
       }

       runnable_t *task = dequeue(pool->runnables);

       // FIXME
       if(sem_post(&(pool->pool_mutex))) return semaphore_error;

       (*(task->function))(task->arg, task->argsz);
       free(task);
   }
}