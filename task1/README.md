# Task 1 — Multi-threaded Array Sum



### Objective



To calculate the sum of all elements of a randomly generated array of 64-bit unsigned integers using four threads and compare two different ways of distributing the work among the threads with the normal single-threaded approach.



The program was written in C using the POSIX pthreads API and was executed on Ubuntu through WSL and an Android device through Termux.



### Implementation



Array of 10,000,000 elements, type uint64\_t.



Three approaches were implemented:



###### 1\. Single-threaded



A single thread iterates through the complete array and adds every element to the total sum.



###### 2\. Strategy 1 — Interleaved elements



For a thread with index i, the thread processes elements whose indices satisfy: x % 4 == i.



The four threads access the array in an interleaved manner.



Thread 0: 0, 4, 8, 12, ...

Thread 1: 1, 5, 9, 13, ...

Thread 2: 2, 6, 10, 14, ...

Thread 3: 3, 7, 11, 15, ...



Each thread maintains its own partial sum which is combined after all four threads finish.



###### 3\. Strategy 2 — Contiguous ranges



The array is divided into four contiguous sections.



For thread i, the range is: i \* N / 4  →  (i + 1) \* N / 4



Thus, each thread works on one continuous portion of the array.



Thread 0: first quarter, Thread 1: second quarter, Thread 2: third quarter, Thread 3: fourth quarter/



Again, each thread calculates its own partial sum and the partial sums are combined after joining all threads.



### Correctness



The same array was used for all three approaches by generating it once using fixed seed srand(42) to compare the results directly.



The sums obtained were:



Single-threaded sum : 10736046639121121

Strategy 1 sum      : 10736046639121121

Strategy 2 sum      : 10736046639121121



All three sums matched, confirming that both multi-threaded strategies produced the correct result.



### Performance



The program was executed through Ubuntu through WSL and Termux on Android.



The observed execution times were:



(Ubuntu)

Single-threaded time : 8.921 ms

Strategy 1 time : 14.516 ms

Strategy 2 time : 3.730 ms



(Termux)

Single-threaded time : 5.537 ms

Strategy 1 time      : 22.076 ms

Strategy 2 time      : 5.775 ms



Strategy 2 had the lowest execution time among the two multi-threaded approaches.



### Observations



The main difference between the two strategies is **the way they access memory**.



In Strategy 1, every thread accesses elements with a stride of four. This results in interleaved memory access, which is less cache-friendly. The threads also have to perform the modulo-based indexing pattern while processing the array.



In Strategy 2, every thread processes a contiguous section of the array. This gives more sequential memory access and makes better use of the CPU cache and memory subsystem.



The results showed that Strategy 1 was significantly slower than both the single-threaded approach and Strategy 2. Strategy 2 performed much better because of its contiguous memory access pattern.



The results also show that using multiple threads does not automatically make a program faster. Summing an array is a relatively simple and memory-intensive operation, so the overhead of creating and joining threads can reduce the benefit of parallelism. In some runs, the single-threaded version was slightly faster than Strategy 2.



### What I learned



* Parallelizing a computation is not only about dividing the work between threads. The way the threads access memory can have a significant effect on performance.
* Creating and synchronizing pthreads, divide work between threads, using per-thread partial results to avoid unnecessary shared-state synchronization, and measuring execution time on an actual Linux environment.
* Importance of memory locality and cache-friendly access patterns in parallel programs.

