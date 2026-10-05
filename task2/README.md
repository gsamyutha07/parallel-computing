# Task 2 — Lock-Free SPSC Ring Buffer



### Objective



To implement a lock-free **Single Producer Single Consumer (SPSC) ring buffer** using a fixed-size array, C11 threads, and atomic operations.



The program was written in C using threads.h and stdatomic.h and was executed through Termux on Android.



### Implementation



Ring buffer of size 8, tested it by producing and consuming 20 integer values.



The buffer contains:



data\[]

head

tail



where **head** keeps track of the producer's position and **tail** keeps track of the consumer's position.



Only the producer modifies head and only the consumer modifies tail, allowing the queue to work without using a mutex.



##### Adding elements



The producer:



1\. Checks if the buffer is full.

2\. Calculates the array index.

3\. Writes the value.

4\. Updates head.



##### Removing elements



The consumer:



1\. Checks if the buffer is empty.

2\. Calculates the array index.

3\. Reads the value.

4\. Updates tail.



Since the buffer size is a power of 2, I used:



**index = counter \& (BUFFER\_SIZE - 1);**



for circular indexing. For a buffer size of 8, this produces indices 0, 1, 2, ..., 7, 0, 1, ..., equivalent to **counter % BUFFER\_SIZE**.



### Atomic Operations



**head** and **tail** were implemented as atomic variables using **atomic\_size\_t**.



I used relaxed loads for the thread's own counter and acquire/release operations when communicating the updated positions between the producer and consumer.



Producer: write data → release head

Consumer: acquire head → read data



This ensures that the consumer only reads an element after the producer has finished writing it.



### Compilation



The program was compiled using:



clang -Wall -Wextra -O2 -std=c11 task2.c -o task2



### Testing



The program was run multiple times to check the behaviour of the producer and consumer threads.



Example output:



Added   : 1

Removed : 1

Added   : 2

Added   : 3

Added   : 4

Removed : 2

...

Added   : 20

Removed : 20



All items produced and consumed successfully.



The order in which the **Added** and **Removed** messages appeared changed between runs because of thread scheduling.



However, the values were always removed in the same order in which they were added, confirming FIFO behaviour.



### Observations



* The buffer was able to fill up and reuse positions after the consumer removed elements, demonstrating the circular nature of the buffer.
* No mutex was used. The producer and consumer communicate through the atomic **head** and **tail** variables.
* The changing order of the terminal messages between runs is expected because the two threads can be scheduled differently.



### What I learned



* How a lock-free SPSC queue can be implemented using atomic operations instead of mutexes.
* How **head** and **tail** are used to manage a circular buffer, how power-of-two bit masking can be used for indexing, and why acquire/release memory ordering is needed when sharing data between threads.

