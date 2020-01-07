#include <stdio.h>

#include "threadpool.h"
#include "future.h"

long long number(long long x) {
    return x;
}

long long multiplicate_number(long long x, long long what) {
    return x * what;
}

typedef struct args {
    long long x;
} args_t;

typedef struct double_args {
    long long x;
    long long y
} double_args_t;

void *callable_regular_number(void *args, size_t argsz) {
    args_t *args_temp = (args_t *) args;
    return (void *) number(args_temp->x);
}

int main() {
    long long n;
    scanf("%lld", &n);

    thread_pool_t *my_pool = malloc(sizeof(thread_pool_t));
    thread_pool_init(my_pool, 3);

    callable_t *my_callable[n + 1];
    future_t *future[n + 1];
    for (int i = 1; i <= n; i++) {
        future[i] = malloc(sizeof(future_t));
        my_callable[i] = malloc(sizeof(callable_t));
        if (i <= 3) {
            args_t* arg = malloc(sizeof(args_t));
            arg->x = i;
            my_callable[i]->function = callable_regular_number;
            my_callable[i]->arg = (void *)arg;
        }
    }

    async(my_pool, future[1], *my_callable[1]);

    return 0;
}