#include <stdio.h>

#include "threadpool.h"
#include "future.h"

typedef struct arguments {
    long long x;
    long long limit;
} arg_t;

void* callable_number(void* arg, size_t size, size_t* res_size) {
    *res_size = sizeof(long long);
    arg_t* my_arg = arg;
    long long liczba = my_arg->x;
    long long wynik = my_arg->x;
    while(liczba <= my_arg->limit) {
        wynik *= liczba;
        liczba += 3;
    }
    return (void *)wynik;
}

int main() {

    long long n;
    scanf("%lld", &n);

    thread_pool_t *my_pool = malloc(sizeof(thread_pool_t));
    thread_pool_init(my_pool, 3);

    future_t *future1 = malloc(sizeof(future_t));
    future_t *future2 = malloc(sizeof(future_t));
    future_t *future3 = malloc(sizeof(future_t));

    arg_t *my_arg_1 = malloc(sizeof(arg_t));
    my_arg_1->x = 1;
    my_arg_1->limit = n;

    arg_t *my_arg_2 = malloc(sizeof(arg_t));
    my_arg_2->x = 1;
    my_arg_2->limit = n;

    arg_t *my_arg_3 = malloc(sizeof(arg_t));
    my_arg_3->x = 1;
    my_arg_3->limit = n;

    callable_t my_callable;// = malloc(sizeof(callable_t));
    my_callable.function = callable_number;
    my_callable.arg = my_arg_1;
    my_callable.argsz = sizeof(arg_t);

    async(my_pool, future1, my_callable);

    void *a = await(future1);
    long long *avar = a;
    printf("%lld", *avar);

    thread_pool_destroy(my_pool);
    free(my_pool);
    return 0;
}