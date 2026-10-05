#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 10000000
#define NUM_THREADS 4

typedef struct
{
    uint64_t *array;
    int thread_id;
    uint64_t partial_sum;
} ThreadData;

/* Calculate elapsed time in milliseconds */
double elapsed_ms(struct timespec start, struct timespec end)
{
    return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
}

/* Single-threaded sum */
uint64_t single_thread_sum(uint64_t *array)
{
    uint64_t sum = 0;
    for (int i = 0; i < N; i++)
    {
        sum += array[i];
    }
    return sum;
}

/* Strategy 1:
   Thread i processes indices:
   i, i + 4, i + 8, ...
*/
void* strategy1_worker(void* arg)
{
    ThreadData *data = (ThreadData *)arg;
    uint64_t sum = 0;
    for (int i = data->thread_id; i < N; i += NUM_THREADS)
    {
        sum += data->array[i];
    }
    data->partial_sum = sum;
    return NULL;
}

/* Strategy 2:
   Thread i processes a contiguous range:
   [i*N/4, (i+1)*N/4)
*/
void* strategy2_worker(void* arg)
{
    ThreadData *data = (ThreadData *)arg;
    uint64_t sum = 0;
    int start = data->thread_id * N / NUM_THREADS;
    int end = (data->thread_id + 1) * N / NUM_THREADS;
    for (int i = start; i < end; i++)
    {
        sum += data->array[i];
    }
    data->partial_sum = sum;
    return NULL;
}

int main()
{
    uint64_t *array = malloc(N * sizeof(uint64_t));
    if (array == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    /* Generate random array */
    srand(42);

    for (int i = 0; i < N; i++)
    {
        array[i] = rand();
    }

    // Single-threaded
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    uint64_t single_sum = single_thread_sum(array);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double single_time = elapsed_ms(start, end);

    // Strategy 1:
    pthread_t threads1[NUM_THREADS];
    ThreadData data1[NUM_THREADS];
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        data1[i].array = array;
        data1[i].thread_id = i;
        data1[i].partial_sum = 0;
        pthread_create(&threads1[i],NULL,strategy1_worker,&data1[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads1[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double strategy1_time = elapsed_ms(start, end);
    uint64_t strategy1_sum = 0;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        strategy1_sum += data1[i].partial_sum;
    }

    // Strategy 2:
    pthread_t threads2[NUM_THREADS];
    ThreadData data2[NUM_THREADS];
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        data2[i].array = array;
        data2[i].thread_id = i;
        data2[i].partial_sum = 0;
        pthread_create(&threads2[i],NULL,strategy2_worker,&data2[i]);
    }
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads2[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double strategy2_time = elapsed_ms(start, end);
    uint64_t strategy2_sum = 0;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        strategy2_sum += data2[i].partial_sum;
    }


    //Results:
    printf("\n========== RESULTS ==========\n");
    printf("Single-threaded sum : %llu\n",
           (unsigned long long)single_sum);
    printf("Strategy 1 sum      : %llu\n",
           (unsigned long long)strategy1_sum);
    printf("Strategy 2 sum      : %llu\n",
           (unsigned long long)strategy2_sum);
    printf("\n");

    printf("Single-threaded time : %.3f ms\n", single_time);
    printf("Strategy 1 time      : %.3f ms\n", strategy1_time);
    printf("Strategy 2 time      : %.3f ms\n", strategy2_time);
    printf("\n");

    if (single_sum == strategy1_sum && single_sum == strategy2_sum)
    {
        printf("All sums match: YES\n");
    }
    else
    {
        printf("All sums match: NO\n");
    }

    free(array);

    return 0;
}
