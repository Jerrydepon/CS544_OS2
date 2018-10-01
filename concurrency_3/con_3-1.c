// Chih-Hsiang Wang
// 05/13/2018

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

pthread_mutex_t mux_1, mux_2;

int current = 0;
int random_func(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

// ==================================================
void *processes_func() {
    while (1) {
        int exec_time = 0;
        pthread_mutex_lock(&mux_1);
        // create new proccess
        exec_time = random_func(1, 7);
        current++;
        printf("New process is running for %d seconds. Total number of process is %d.\n", exec_time, current);

        // check if there are three proccesses
        if (current == 3) {
            printf("Wait for three processes to finish.\n");
        } else {
          pthread_mutex_unlock(&mux_1);
        }

        // proccessing period
        sleep(exec_time);
        printf("Number %d process is finished.\n", current--);

        pthread_mutex_lock(&mux_2);
        // to make sure all three procceses are finished before new proccess
        if (current == 0) {
            printf("All three finished. Able for new proccess\n");
            pthread_mutex_unlock(&mux_1);
        }
        exec_time = random_func(2, 6);
        pthread_mutex_unlock(&mux_2);
        sleep(exec_time);
    }
}

// ==================================================
int main() {
    srand(time(NULL));

    // thread initial
    pthread_t thread_arr[3];
    pthread_mutex_init(&mux_1, NULL);
    pthread_mutex_init(&mux_2, NULL);

    // create threads
    int i;
    for (i = 0; i < 3; i++) {
        pthread_create(&thread_arr[i], NULL, processes_func, NULL);
    }

    // wait for created thread to exit
    for (i = 0; i < 3; i++) {
        pthread_join(thread_arr[i], NULL);
    }

    return 0;
}
