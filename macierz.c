#include <stdio.h>
#include <unistd.h>

#include "threadpool.h"

typedef struct arg {
    int x;
    int value;
    int time;
    sem_t *semaphores;
    long long *result;
} arg_t;


void add_val(int x, int value, int time, sem_t* semaphores, long long *result) {
    usleep(time*1000);
    sem_wait(&(semaphores[x]));
    result[x] += value;
    sem_post(&(semaphores[x]));
}

void count_runnable(void *arg_temp, size_t argsz __attribute__((unused))) {
    arg_t *arg = (arg_t *) arg_temp;
    add_val(arg->x, arg->value, arg->time, arg->semaphores, arg->result);
}


int main () {
    int n, k;
    scanf("%d", &k);
    scanf("%d", &n);

    int times[k][n];
    int values[k][n];
    long long result[k];

    for(int i = 0; i < k; i++){
        for(int j = 0; j < n; j++) {
            scanf("%d", &values[i][j]);
            scanf("%d", &times[i][j]);
        }
    }

    thread_pool_t *pool = malloc(sizeof(thread_pool_t));
    thread_pool_init(pool, k);

    runnable_t* my_runnables[n * k];
    arg_t* my_arguments[k * n];
    sem_t my_semaphores[k];

    for(int i = 0; i < k; i++) {
        result[i] = 0;
        (sem_init(&(my_semaphores[i]), THREAD_SEMAPHORE, 1));
        for(int j = 0; j < n; j++) {
            my_runnables[i * k + j] = malloc(sizeof(runnable_t));
            my_arguments[i * k + j] = malloc(sizeof(arg_t));

            my_arguments[i * k + j]->x = i;
            my_arguments[i * k + j]->time = times[i][j];
            my_arguments[i * k + j]->value = values[i][j];
            my_arguments[i * k + j]->semaphores = my_semaphores;
            my_arguments[i * k + j]->result = result;

            my_runnables[i * k + j]->function = count_runnable;
            my_runnables[i * k + j]->arg = (void *)my_arguments[i * k + j];
            my_runnables[i * k + j]->argsz = sizeof(arg_t);
        }
    }

    for(int i = 0; i < n * k; i++) {
        defer(pool, *my_runnables[i]);
    }

    thread_pool_destroy(pool);
    for(int i = 0; i < k; i++) {
        free(my_runnables[i]);
        free(my_arguments[i]);
    }
    free(pool);

    for(int i = 0; i < k; i++) {
        printf("%lld\n", result[i]);
    }

    return 0;
}

/*
2
3
1 2
1 5
12 4
23 9
3 11
7 2
 */