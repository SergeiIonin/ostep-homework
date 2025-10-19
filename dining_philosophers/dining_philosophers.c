#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "common_threads.h"

#define NUM_PHILOSOPHERS 5
#define THINKING_TIME 2
#define EATING_TIME 1
#define SIMULATION_TIME 20

// Philosopher states
typedef enum {
    THINKING,
    HUNGRY,
    EATING
} philosopher_state_t;

// Global variables
pthread_mutex_t forks[NUM_PHILOSOPHERS];
pthread_mutex_t state_mutex;
philosopher_state_t state[NUM_PHILOSOPHERS]; // it creates an array
pthread_cond_t self[NUM_PHILOSOPHERS];
int meals_eaten[NUM_PHILOSOPHERS] = {0};
time_t start_time;

// Function prototypes
void* philosopher(void* arg);
void pickup_forks(int philosopher_id);
void putdown_forks(int philosopher_id);
void test(int philosopher_id);
void think(int philosopher_id);
void eat(int philosopher_id);
int left_fork(int philosopher_id);
int right_fork(int philosopher_id);
void print_state();

int left_fork(int philosopher_id) {
    return philosopher_id;
}

int right_fork(int philosopher_id) {
    return (philosopher_id + 1) % NUM_PHILOSOPHERS;
}

void print_state() {
    printf("[%.2f] State: ", difftime(time(NULL), start_time));
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        switch (state[i]) {
            case THINKING: printf("T "); break;
            case HUNGRY:   printf("H "); break;
            case EATING:   printf("E "); break;
        }
    }
    printf("| Meals: ");
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        printf("%d ", meals_eaten[i]);
    }
    printf("\n");
}

void think(int philosopher_id) {
    printf("Philosopher %d is thinking...\n", philosopher_id);
    sleep(rand() % THINKING_TIME + 1);
}

void eat(int philosopher_id) {
    printf("Philosopher %d is eating (meal #%d)\n", philosopher_id, meals_eaten[philosopher_id] + 1);
    meals_eaten[philosopher_id]++;
    sleep(rand() % EATING_TIME + 1);
}

void test(int philosopher_id) {
    if (state[philosopher_id] == HUNGRY &&
        state[left_fork(philosopher_id)] != EATING &&
        state[right_fork(philosopher_id)] != EATING) {
        
        state[philosopher_id] = EATING;
        printf("Philosopher %d picked up forks %d and %d\n", 
               philosopher_id, left_fork(philosopher_id), right_fork(philosopher_id));
        print_state();
        pthread_cond_signal(&self[philosopher_id]);
    }
}

void pickup_forks(int philosopher_id) {
    Mutex_lock(&state_mutex);
    state[philosopher_id] = HUNGRY;
    printf("Philosopher %d is hungry\n", philosopher_id);
    print_state();
    
    test(philosopher_id);
    
    while (state[philosopher_id] != EATING) {
        pthread_cond_wait(&self[philosopher_id], &state_mutex);
    }
    
    Mutex_unlock(&state_mutex);
}

void putdown_forks(int philosopher_id) {
    Mutex_lock(&state_mutex);
    
    state[philosopher_id] = THINKING;
    printf("Philosopher %d put down forks %d and %d\n", 
           philosopher_id, left_fork(philosopher_id), right_fork(philosopher_id));
    print_state();
    
    // Test left and right neighbors
    test((philosopher_id + NUM_PHILOSOPHERS - 1) % NUM_PHILOSOPHERS);
    test((philosopher_id + 1) % NUM_PHILOSOPHERS);
    
    Mutex_unlock(&state_mutex);
}

void* philosopher(void* arg) {
    int philosopher_id = *(int*)arg;
    
    printf("Philosopher %d started\n", philosopher_id);
    
    while (difftime(time(NULL), start_time) < SIMULATION_TIME) {
        think(philosopher_id);
        pickup_forks(philosopher_id);
        eat(philosopher_id);
        putdown_forks(philosopher_id);
    }
    
    printf("Philosopher %d finished with %d meals\n", philosopher_id, meals_eaten[philosopher_id]);
    return NULL;
}

int main(void) {
    pthread_t philosophers[NUM_PHILOSOPHERS];
    int philosopher_ids[NUM_PHILOSOPHERS];
    
    // Seed random number generator
    srand(time(NULL));
    start_time = time(NULL);
    
    printf("=== Dining Philosophers Problem ===\n");
    printf("Number of philosophers: %d\n", NUM_PHILOSOPHERS);
    printf("Simulation time: %d seconds\n", SIMULATION_TIME);
    printf("Legend: T=Thinking, H=Hungry, E=Eating\n\n");
    
    // Initialize mutexes and condition variables
    Mutex_init(&state_mutex);
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        Mutex_init(&forks[i]);
        Cond_init(&self[i]);
        state[i] = THINKING;
        philosopher_ids[i] = i;
    }
    
    print_state();
    
    // Create philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        Pthread_create(&philosophers[i], NULL, philosopher, &philosopher_ids[i]);
    }
    
    // Wait for all philosophers to finish
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        Pthread_join(philosophers[i], NULL);
    }
    
    // Print final statistics
    printf("\n=== Final Results ===\n");
    int total_meals = 0;
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        printf("Philosopher %d ate %d meals\n", i, meals_eaten[i]);
        total_meals += meals_eaten[i];
    }
    printf("Total meals served: %d\n", total_meals);
    printf("Average meals per philosopher: %.2f\n", (double)total_meals / NUM_PHILOSOPHERS);
    
    return 0;
}
