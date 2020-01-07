#include <stdio.h>
#include <unistd.h>

#include "threadpool.h"

typedef struct arg {
    int *val;
    int *times;
    int k;
    int n;
    long long *res;
} arg_t;

void count (int *values, int *times, int k, int n, long long *result) {
    long long wynik = 0;
    for(int i = 0; i < n; i++) {
        usleep(times[i] * 1000);
        wynik += values[i];
    }
    result[k] = wynik;
}

void count_runnable(void *arg_temp, size_t argsz) {
    arg_t *arg = (arg_t *) arg_temp;
    count(arg->val, arg->times, arg->k, arg->n, arg->res);
}

int main () {
    int n, k;
    scanf("%d", &k);
    scanf("%d", &n);

    printf("chuj ");

    int times[k][n];
    int values[k][n];
    long long result[k];

    for(int i = 0; i < k; i++){
        for(int j = 0; j < n; j++) {
            scanf("%d", &values[i][j]);
            scanf("%d", &times[i][j]);
        }
    }
    /*tu threadpoo; */

    thread_pool_t *pool = malloc(sizeof(thread_pool_t));
    thread_pool_init(pool, k);

    runnable_t* my_runnables[k];
    arg_t* my_arguments[k];

    for(int i = 0; i < k; i++) {
        my_runnables[i] = malloc(sizeof(runnable_t));
        my_arguments[i] = malloc(sizeof(arg_t));

        my_arguments[i]->times = times[i];
        my_arguments[i]->val = values[i];
        my_arguments[i]->n = n;
        my_arguments[i]->k = i;
        my_arguments[i]->res = result;

        my_runnables[i]->function = count_runnable;
        my_runnables[i]->arg = (void *)my_arguments;
        my_runnables[i]->argsz = sizeof(arg_t);
    }

    printf("chuj\n");
    for(int i = 0; i < k; i++) {
        defer(pool, *my_runnables[i]);
    }

    printf("japierdole\n");
    thread_pool_destroy(pool);
    printf("jebaneścierwo\n");
    /* tu threadpool */
//    for(int i = 0; i < k; i++) {
//        count(values[i], times[i], i, n, result);
//    }

    for(int i = 0; i < k; i++) {
        printf("%d\n", result[i]);
    }

}