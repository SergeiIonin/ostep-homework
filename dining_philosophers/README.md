# Dining Philosophers Problem

## Overview

The **Dining Philosophers Problem** is a classic synchronization problem in computer science that illustrates the challenges of resource allocation and deadlock avoidance in concurrent programming. It was originally formulated by Edsger Dijkstra in 1965.

## The Problem

Five philosophers sit around a circular table with five chopsticks (or forks) placed between them. Each philosopher alternates between thinking and eating. To eat, a philosopher needs both chopsticks adjacent to them (left and right). The challenge is to design a protocol that prevents deadlock and starvation while allowing maximum concurrency.

### Potential Issues

1. **Deadlock**: All philosophers pick up their left chopstick simultaneously, then wait for the right one
2. **Starvation**: Some philosophers may never get to eat if others monopolize resources
3. **Race Conditions**: Concurrent access to shared resources without proper synchronization

## Solution Approach

This implementation uses the **Monitor Solution** with condition variables:

- **State Tracking**: Each philosopher has a state (THINKING, HUNGRY, EATING)
- **Mutual Exclusion**: A single mutex protects the state array
- **Condition Variables**: Each philosopher waits on their own condition variable
- **Test Function**: Checks if a philosopher can transition to eating state

### Key Features

- **Deadlock-free**: No circular waiting
- **Starvation-free**: Fair access through condition variables
- **High Concurrency**: Multiple philosophers can eat simultaneously if resources allow
- **Visual Output**: Real-time state display and statistics

## Building and Running

### Prerequisites

- GCC compiler with pthread support
- POSIX-compliant system (Linux, macOS, Unix)

### Build Commands

```bash
# Build the program
make

# Build and run
make run

# Clean compiled files
make clean

# Debug build
make debug

# Release build (optimized)
make release

# Run with memory leak detection
make valgrind

# Run with thread race condition detection
make helgrind

# Show all available commands
make help
```

### Example Output

```
=== Dining Philosophers Problem ===
Number of philosophers: 5
Simulation time: 20 seconds
Legend: T=Thinking, H=Hungry, E=Eating

[0.00] State: T T T T T | Meals: 0 0 0 0 0
Philosopher 0 started
Philosopher 1 started
...
Philosopher 2 is thinking...
Philosopher 0 is hungry
[2.45] State: H T T T T | Meals: 0 0 0 0 0
Philosopher 0 picked up forks 0 and 1
[2.45] State: E T T T T | Meals: 0 0 0 0 0
...
```

## Thread Theory in C

### What are Threads?

**Threads** are lightweight processes that share memory space but execute independently. In C, threads are typically implemented using the POSIX threads (pthreads) library.

#### Thread vs Process

| Aspect | Thread | Process |
|--------|--------|---------|
| Memory | Shared address space | Separate address space |
| Creation Cost | Low | High |
| Communication | Shared memory | IPC mechanisms |
| Context Switch | Fast | Slow |
| Isolation | Low | High |

### Thread Lifecycle

```
CREATE → READY → RUNNING → BLOCKED/WAITING → TERMINATED
```

1. **CREATE**: Thread is created but not yet scheduled
2. **READY**: Thread is ready to run, waiting for CPU
3. **RUNNING**: Thread is executing on CPU
4. **BLOCKED**: Thread is waiting for resource/event
5. **TERMINATED**: Thread has finished execution

### POSIX Threads (pthreads) API

#### Core Functions

```c
// Thread management
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg);
int pthread_join(pthread_t thread, void **retval);
int pthread_detach(pthread_t thread);
void pthread_exit(void *retval);

// Synchronization - Mutexes
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);

// Synchronization - Condition Variables
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
```

#### Our Wrapper Macros

The `common_threads.h` header provides error-checking wrappers:

```c
#define Pthread_create(thread, attr, start_routine, arg) \
    assert(pthread_create(thread, attr, start_routine, arg) == 0);

#define Mutex_lock(m) assert(pthread_mutex_lock(m) == 0);
#define Mutex_unlock(m) assert(pthread_mutex_unlock(m) == 0);
```

### How C Threads Execute on Host OS

#### Thread Models

1. **User-Level Threads (N:1)**
   - Multiple user threads mapped to one kernel thread
   - Fast context switching, but no true parallelism
   - Blocking system calls block entire process

2. **Kernel-Level Threads (1:1)**
   - Each user thread maps to one kernel thread
   - True parallelism on multi-core systems
   - Higher overhead for creation/switching

3. **Hybrid Model (M:N)**
   - M user threads mapped to N kernel threads
   - Balances performance and parallelism
   - Complex implementation

#### Modern Implementation (Linux/macOS)

Most modern systems use the **1:1 model**:

- **Linux**: Native POSIX Threads Library (NPTL)
- **macOS**: pthread implementation over Mach threads
- **Windows**: pthreads-win32 library

#### Thread Scheduling

The OS scheduler manages thread execution:

1. **Preemptive Scheduling**: OS can interrupt threads
2. **Time Slicing**: Each thread gets CPU time quantum
3. **Priority-based**: Higher priority threads run first
4. **SMP Support**: Threads can run on different CPU cores

#### Memory Model

Threads share:
- **Code segment**: Program instructions
- **Data segment**: Global and static variables
- **Heap**: Dynamic memory allocation

Each thread has its own:
- **Stack**: Local variables and function calls
- **Registers**: CPU register state
- **Program Counter**: Current instruction pointer

### Synchronization Primitives

#### Mutexes (Mutual Exclusion)

```c
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_lock(&lock);
// Critical section
pthread_mutex_unlock(&lock);
```

#### Condition Variables

```c
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Wait for condition
pthread_mutex_lock(&mutex);
while (!condition_is_true) {
    pthread_cond_wait(&condition, &mutex);
}
pthread_mutex_unlock(&mutex);

// Signal condition
pthread_mutex_lock(&mutex);
condition_is_true = 1;
pthread_cond_signal(&condition);
pthread_mutex_unlock(&mutex);
```

### Common Threading Issues

#### Race Conditions

Multiple threads accessing shared data simultaneously:

```c
// Problematic code
int counter = 0;
void* increment(void* arg) {
    for (int i = 0; i < 1000000; i++) {
        counter++;  // Not atomic!
    }
    return NULL;
}
```

#### Deadlock

Circular dependency on resources:

```c
// Thread 1                 // Thread 2
lock(mutex1);               lock(mutex2);
lock(mutex2);  // Wait      lock(mutex1);  // Wait
```

#### Starvation

A thread never gets access to required resources due to scheduling or priority issues.

### Best Practices

1. **Always Check Return Values**: pthread functions return error codes
2. **Use Wrappers**: Like our `common_threads.h` for error handling
3. **Minimize Critical Sections**: Hold locks for shortest time possible
4. **Avoid Nested Locks**: Prevents deadlock
5. **Use Condition Variables**: For complex waiting conditions
6. **Clean Up Resources**: Destroy mutexes and condition variables
7. **Thread-Safe Functions**: Use reentrant versions (_r functions)

### Performance Considerations

- **Context Switching Overhead**: Don't create too many threads
- **Cache Effects**: Thread migration between cores affects performance
- **False Sharing**: Multiple threads accessing nearby memory locations
- **Lock Contention**: High contention reduces parallelism benefits

## Further Reading

- **Books**:
  - "Operating Systems: Three Easy Pieces" by Remzi Arpaci-Dusseau
  - "Programming with POSIX Threads" by David Butenhof
  - "The Art of Multiprocessor Programming" by Herlihy & Shavit

- **Standards**:
  - POSIX.1-2008 (IEEE Std 1003.1-2008)
  - C11 threads (alternative to pthreads)

- **Online Resources**:
  - [pthread Tutorial](https://computing.llnl.gov/tutorials/pthreads/)
  - [Linux Programmer's Manual](https://man7.org/linux/man-pages/)

## License

This implementation is for educational purposes as part of the OSTEP homework series.
