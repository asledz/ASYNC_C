#include <stdio.h>

#include "threadpool.h"
#include "future.h"

int main () {
    int n, k;
    scanf("%d", &k);
    scanf("%d", &n);
    printf("%d", n*k);
}