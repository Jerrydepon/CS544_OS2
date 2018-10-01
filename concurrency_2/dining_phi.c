// Chih-Hsiang Wang

// gcc -o dining_phi dining_phi.c -lpthread
// Reference: https://www.youtube.com/watch?v=syMOLWlGjNg
// Reference: https://www.geeksforgeeks.org/operating-system-dining-philosopher-problem-using-semaphores/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define N 5
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define LEFT (idx + 4) % N
#define RIGHT (idx + 1) % N

sem_t mutex;
sem_t sem_arr[N];

int state[N];
char *phi_arr[N] = { "Plato", "Buddha", "Confucious", "Descartes", "Aristotle" };
int number[N] = {0, 1, 2, 3, 4};

// ==================================================
void test(int idx) {
    if (state[idx] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        // change into eating state
        state[idx] = EATING;
        printf("%s takes fork %d and %d\n", phi_arr[idx], LEFT + 1, idx + 1);

        // signal to the philoshoper to eat
        sem_post(&sem_arr[idx]);
    }
}

void put_forks(int idx) {
    // lock mutex
    sem_wait(&mutex);

    // change into thinking state
    state[idx] = THINKING;
    printf("%s puts fork %d and %d down\n", phi_arr[idx], LEFT + 1, idx + 1);

    // if the philosopher beside is hungry & there are forks to use, let them eat
    test(LEFT);
    test(RIGHT);

    // unlock mutex
    sem_post(&mutex);
}

void take_forks(int idx) {
    // lock mutex
    sem_wait(&mutex);

    // set to state hungry
    state[idx] = HUNGRY;
    printf("%s is hungry\n", phi_arr[idx]);

    // check if there are forks to use
    test(idx);

    // unlock mutex
    sem_post(&mutex);

    // wait until there are forks to eat
    sem_wait(&sem_arr[idx]);
}

void *philosopher(void *num) {
    while(true) {
        int *idx = num;

        // thinking, 1~20
        printf("%s is thinking\n", phi_arr[*idx]);
        int r = rand() % 20 + 1;
        sleep(r);

        // take fork
        take_forks(*idx);

        // eating, 2~9
        printf("%s is eating\n", phi_arr[*idx]);
        r = rand() % 8 + 2;
        sleep(r);

        // put fork
        put_forks(*idx);
    }
}

// ==================================================
int main() {
    int i;
    pthread_t tid[N];
    srand(time(NULL));

    // initialize semaphores
    // int sem_init(sem_t *mutex, int pshared, unsigned int value);
    // pshared = 0, semaphore is shared between the threads of a process
    sem_init(&mutex, 0, 1);
    for (i = 0; i < N; i++) {
        sem_init(&sem_arr[i], 0, 0);
    }

    // create N philosopher threads
    for (i = 0; i < N; i++) {
        pthread_create(&tid[i], NULL, philosopher, &number[i]);
    }

    // wait for created thread to exit
    for (i = 0; i < N; i++) {
        pthread_join(tid[i], NULL);
    }
}
