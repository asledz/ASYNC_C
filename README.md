# ASYNC_C

Implementation of threadpool and async/await mechanism on it in C.

Project for university, concurrent programming.

## Threadpool
The thread pool is the implementation of the interface presented in the [threadpool.h](threadpool.h) file.

```
int thread_pool_init(thread_pool_t *pool, size_t pool_size);
```
Creates a thread_pool_t object at argument pool, with number of thread passed as pool_size argument.

```
void thread_pool_destroy(thread_pool_t *pool);
```
Destroys pool, after finishing all of the tasks already assigned.

```
defer(thread_pool_t *pool, runnable_t runnable);
```
Assigns new task runnable to the pool.

## Future (async/await mechanism)
Thre future is the implementation of the interface presented in the [future.h](future.h) file.

```
int async(thread_pool_t *pool, future_t *future, callable_t callable);
```
Initializes memory for future, assigns callable to calculate the value in the pool and return it to future mechanism.

```
void *await(future_t *future);
```
Waits for result assigned to future to be caluclated.

```
int map(thread_pool_t *pool, future_t *future, future_t *from,
        void *(*function)(void *, size_t, size_t *));
```
Maps value from the future from to the fututre future with given function, using threadpool.

## Aditional files.

- [queue.h](queue.h) and [queue.c](queue.h) - implementation of generic queue.
- [matrix](macierz.c) - uses pool to calculate the sum of rows in the matrix
- [factorial](silnia.c) - not finished, calculates factorial with usage of future mechanism.
- [test](/test) - some tests given in the task.
