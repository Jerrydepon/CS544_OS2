// Chih-Hsiang Wang
// 31/05/2018

// Reference: https://www.youtube.com/watch?v=OvJFpsN5czg&t=471s
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

sem_t custReady;
sem_t accessSeat;
sem_t barberReady;
int num_chair;
int total_num_chair;
int cust_waiting = 0; // 1: there is customer waiting

int randNumber(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

void *barber() {
    while (1) {
        // sleep if there is no customer waiting
        if (cust_waiting == 0 && num_chair == total_num_chair) {
            printf("The barber is sleeping.\n");
        }
        sem_wait(&custReady);
        sem_wait(&accessSeat);

        num_chair++;

        sem_post(&barberReady);
        sem_post(&accessSeat);

        // cut the hair
        printf("The barber is ready to cut hair.\n");
        sleep(randNumber(2, 7)); // time for cutting hair
    }
}

void *customer(void *arg) {
    int tag_customer = *((int *)arg);
    while (1) {
        sem_wait(&accessSeat);
        if (num_chair > 0) {
            num_chair--;
            cust_waiting = 1;
            printf("Customer #%d is waiting on the chair.\n", tag_customer);

            sem_post(&custReady);
            sem_post(&accessSeat);
            sem_wait(&barberReady);

            printf("The barber is cutting hair for customer #%d.\n", tag_customer);
            cust_waiting = 0;
            sleep(randNumber(6, 10)); // time interval for next customer
        }
        else {
            printf("Customer #%d leaves because there is no chair.\n", tag_customer);
            sem_post(&accessSeat);
            sleep(randNumber(6, 10)); // time interval for next customer
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid number of arguments\n");
        exit(1);
    }

    srand(time(NULL));
    int i, j;
    int num_chair_par = atoi(argv[1]);
    int cust_num_par = atoi(argv[2]);
    int args[cust_num_par];
    pthread_t barber_thread;
    pthread_t customers_thread[cust_num_par];

    num_chair = num_chair_par;
    total_num_chair = num_chair;

    sem_init(&custReady, 0, 0);
    sem_init(&accessSeat, 0, 1);
    sem_init(&barberReady, 0, 0);

    for (i = 0; i < cust_num_par; i++) {
        args[i] = i;
    }

    // create threads
    pthread_create(&barber_thread, NULL, barber, NULL);
    for (i = 0; i < cust_num_par; i++) {
        pthread_create(&customers_thread[i], NULL, customer, (void *)&args[i]);
    }

    // join threads
    pthread_join(barber_thread, NULL);
    for (i = 0; i < cust_num_par; i++) {
        pthread_join(customers_thread[i], NULL);
    }

    return 0;
}
