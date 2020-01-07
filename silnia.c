#include <stdio.h>

#include "threadpool.h"
#include "future.h"

typedef struct arguments {
    long long x
} arg_t;

void* callable_number(void* arg, size_t size, size_t* res_size) {
    *res_size = sizeof(long long);
    arg_t* my_arg = arg;
    return (void *)my_arg->x;
}

int main() {
    long long n;
    scanf("%lld", &n);

    thread_pool_t *my_pool = malloc(sizeof(thread_pool_t));
    thread_pool_init(my_pool, 3);


    future_t *my_future = malloc(sizeof(future_t));
    arg_t *my_arg = malloc(sizeof(arg_t));
    my_arg->x = 1;

    callable_t my_callable;// = malloc(sizeof(callable_t));
    my_callable.function = callable_number;
    my_callable.arg = my_arg;
    my_callable.argsz = sizeof(arg_t);

    async(my_pool, my_future, my_callable);

    scanf("%lld", (long long*)(await(my_future)));
//    callable_t *my_callable[n + 1];
//    future_t *future[n + 1];
//    for (int i = 1; i <= n; i++) {
//        future[i] = malloc(sizeof(future_t));
//        my_callable[i] = malloc(sizeof(callable_t));
//    }

//    async(my_pool, future[1], *my_callable[1]);

    thread_pool_destroy(my_pool);
    free(my_pool);
    return 0;
}