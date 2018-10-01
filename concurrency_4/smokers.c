// Chih-Hsiang Wang
// 31/05/2018

// Reference: https://cse.yeditepe.edu.tr/%7Ekserdaroglu/spring2014/cse331/labnotes/WEEK%205%20-%20SEMAPHORES/mysemaphoreexamplesMOE.pdf
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

sem_t sem_smoker[3];
sem_t sem_pusher[3];
pthread_mutex_t mutex_agent;
pthread_mutex_t mutex_pusher;

char *ingredients[3] = {"matches + paper", "paper + tobacco", "tobacco + matches"};
int num_ingred[3] = {0, 0, 0};

int randNumber(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

void *agent(void *arg) {
    int idx_agent = *(int *)arg;

    while (1) {
        sleep(randNumber(1, 4));

        pthread_mutex_lock(&mutex_agent);

        sem_post(&sem_pusher[idx_agent]);
        sem_post(&sem_pusher[(idx_agent + 1) % 3]);

        printf("Agent offers %s.\n", ingredients[idx_agent]);
    }
}

void *pusher(void *arg) {
    int idx_pusher = *(int *)arg;

    while (1) {
        sem_wait(&sem_pusher[idx_pusher]);
        pthread_mutex_lock(&mutex_pusher);

        if (num_ingred[(idx_pusher + 1) % 3]) {
            num_ingred[(idx_pusher + 1) % 3] -= 1;
            sem_post(&sem_smoker[idx_pusher]);
        }
        else if (num_ingred[(idx_pusher + 2) % 3]) {
            num_ingred[(idx_pusher + 2) % 3] -= 1;
            sem_post(&sem_smoker[(idx_pusher + 2) % 3]);
        }
        else {
            num_ingred[idx_pusher] += 1;
        }

        pthread_mutex_unlock(&mutex_pusher);
    }
}

void *smoker(void *arg) {
    int idx_smoker = *(int *)arg;

    while (1) {
        printf("Smoker %d is waiting for %s.\n", idx_smoker, ingredients[idx_smoker]);

        sem_wait(&sem_smoker[idx_smoker]);

        printf("Smoker %d is making a cigarette.\n", idx_smoker);
        sleep(randNumber(3, 6));
        pthread_mutex_unlock(&mutex_agent);

        printf("Smoker %d is smoking.\n", idx_smoker);
        sleep(randNumber(3, 6));
    }
}

int main(int argc, char *arvg[]) {
    srand(time(NULL));

		int args[3];
		pthread_t agent_thread[3];
		pthread_t pusher_thread[3];
		pthread_t smoker_thread[3];

		int i;
    for (i = 0; i < 3; i++) {
        sem_init(&sem_smoker[i], 0, 0);
        sem_init(&sem_pusher[i], 0, 0);
    }

		pthread_mutex_init(&mutex_agent, NULL);
		pthread_mutex_init(&mutex_pusher, NULL);

    for (i = 0; i < 3; i++) {
        args[i] = i;
				pthread_create(&agent_thread[i], NULL, agent, &args[i]);
				pthread_create(&pusher_thread[i], NULL, pusher, &args[i]);
        pthread_create(&smoker_thread[i], NULL, smoker, &args[i]);
    }

    for (i = 0; i < 3; i++) {
				pthread_join(agent_thread[i], NULL);
				pthread_join(pusher_thread[i], NULL);
        pthread_join(smoker_thread[i], NULL);
		}

    return 0;
}
