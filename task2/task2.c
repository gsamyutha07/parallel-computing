#include <stdio.h>
#include <stdbool.h>
#include <threads.h>
#include <stdatomic.h>

#define BUFFER_SIZE 8
#define TOTAL_ITEMS 20

typedef struct {
    int data[BUFFER_SIZE];
    atomic_size_t head;
    atomic_size_t tail;
} RingBuffer;

RingBuffer buffer;

bool ring_buffer_push(RingBuffer *rb, int value)
{
    size_t head = atomic_load_explicit(&rb->head, memory_order_relaxed);

    size_t tail = atomic_load_explicit(&rb->tail, memory_order_acquire);

    if (head - tail >= BUFFER_SIZE)
        return false;

    size_t index = head & (BUFFER_SIZE - 1);

    rb->data[index] = value;

    atomic_store_explicit(&rb->head,head + 1,memory_order_release);

    return true;
}

bool ring_buffer_pop(RingBuffer *rb, int *value)
{
    size_t tail = atomic_load_explicit(&rb->tail, memory_order_relaxed);

    size_t head = atomic_load_explicit(&rb->head, memory_order_acquire);

    if (tail == head)
        return false;

    size_t index = tail & (BUFFER_SIZE - 1);

    *value = rb->data[index];

    atomic_store_explicit(&rb->tail,tail + 1,memory_order_release);

    return true;
}

int producer(void *arg)
{
    RingBuffer *rb = arg;

    for (int i = 1; i <= TOTAL_ITEMS; )
    {
        if (ring_buffer_push(rb, i))
        {
            printf("Added   : %d\n", i);
            i++;
        }

        thrd_yield();
    }

    return 0;
}

int consumer(void *arg)
{
    RingBuffer *rb = arg;

    int value;
    int consumed = 0;

    while (consumed < TOTAL_ITEMS)
    {
        if (ring_buffer_pop(rb, &value))
        {
            printf("Removed : %d\n", value);
            consumed++;
        }

        thrd_yield();
    }

    return 0;
}

int main(void)
{
    thrd_t producer_thread;
    thrd_t consumer_thread;

    atomic_init(&buffer.head, 0);
    atomic_init(&buffer.tail, 0);

    if (thrd_create(&producer_thread, producer, &buffer) != thrd_success)
    {
        fprintf(stderr, "Failed to create producer thread\n");
        return 1;
    }

    if (thrd_create(&consumer_thread, consumer, &buffer) != thrd_success)
    {
        fprintf(stderr, "Failed to create consumer thread\n");
        return 1;
    }

    thrd_join(producer_thread, NULL);
    thrd_join(consumer_thread, NULL);

    printf("\nAll items produced and consumed successfully.\n");

    return 0;
}
