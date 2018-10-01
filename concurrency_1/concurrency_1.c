// CS544
// Chih-Hsiang Wang

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#define BUF_SIZE 32 // size of buffer
// Mersenne Twister
// reference: http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/emt19937ar.html
#include "mt19937ar.h"

// =============== Initilization ===============
struct item {
  int value1;
  int value2;
} buff[BUF_SIZE];

int add = 0; // place to add next element
int rem = 0; // place to remove next element
int items_num = 0; // total number in the buffer
int isRdrand = 0; // determine to use rdrand or not

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; // initilize mutex lock for buffer
pthread_cond_t c_prod = PTHREAD_COND_INITIALIZER; // initilize conditional variable for producer
pthread_cond_t c_cons = PTHREAD_COND_INITIALIZER; // initilize conditional variable for consumer

// =============== Produce Random Number ===============
// check if to use rdrand
void isRdrandCheck() {
  unsigned int eax, ebx, ecx, edx;
  eax = 0x01;
  __asm__ __volatile__(
      "cpuid;"
      : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
      : "a"(eax)
    );

  isRdrand = ecx & 0x40000000;
}

// return random number by rdrand
int rdrand(int min, int max) {
    unsigned int a;
    unsigned char b;

    asm volatile("rdrand %0; setc %1"
                 : "=r"(a), "=qm"(b)
               );
    if (b)
        return ((a % max) + min);
    else
        return 0;
}

// return random number by mt19937
int mt19937(int min, int max) {
    init_genrand(time(NULL));
    int a = (int)genrand_int32();

    return (abs(a) % (max - 1)) + min;
}

// choose random method
int randomNum(int min, int max) {
  if (isRdrand)
    return rdrand(min, max);
  else
    return mt19937(min, max);
}

// =============== Buffer Handling ===============
void storeValue(int val1, int val2) {
  buff[add].value1 = val1;
  buff[add].value2 = val2;
  add = (add+1) % BUF_SIZE;
  items_num++;
}

struct item *removeValue() {
  rem = rem % BUF_SIZE;
  items_num--;
  return &(buff[rem++]);
}

// =============== Threads ===============
void *producerFunc() {
  int val1, val2;
  while(1) {
    pthread_mutex_lock(&lock);
    while (items_num >= BUF_SIZE) {
      pthread_cond_signal(&c_cons);
      pthread_cond_wait(&c_prod, &lock);
    }

    // 1~9
    val1 = randomNum(1, 10);
    // sleep 3~7
    sleep(randomNum(3, 8));
    // 2~9
    val2 = randomNum(2, 9);
    storeValue(val1, val2);

    printf("num: %d, ", items_num);
    printf("producer make: (%d, %d)\n", val1, val2);
    fflush(stdout);

    pthread_cond_signal(&c_cons);
    pthread_cond_wait(&c_prod, &lock);
    pthread_mutex_unlock(&lock);
  }
}

void *consumerFunc() {
  while(1) {
    pthread_mutex_lock(&lock);

    while (items_num <= 0) {
      pthread_cond_signal(&c_prod);
      pthread_cond_wait(&c_cons, &lock);
    }

    struct item *con_item = removeValue();
    sleep(con_item->value2);
    printf("num: %d, ", items_num);
    printf("consumer get: %d\n", con_item->value1);
    fflush(stdout);

    pthread_cond_signal(&c_prod);
    pthread_cond_wait(&c_cons, &lock);
    pthread_mutex_unlock(&lock);
  }
}

// =============== Main ===============
int main() {
  pthread_t tid1, tid2, tid3, tid4, tid5, tid6, tid7; // thread identifiers
  srand(time(NULL));

  // rdrand x86 ASM
  isRdrandCheck();

  // create threads for producer and consumer
  // create three producers
  if (pthread_create(&tid1, NULL, producerFunc, NULL) != 0) {
    fprintf(stderr, "Unable to create producer thread\n");
    exit(1);
  }
  if (pthread_create(&tid2, NULL, producerFunc, NULL) != 0) {
    fprintf(stderr, "Unable to create producer thread\n");
    exit(1);
  }
  if (pthread_create(&tid3, NULL, producerFunc, NULL) != 0) {
    fprintf(stderr, "Unable to create producer thread\n");
    exit(1);
  }
  if (pthread_create(&tid4, NULL, producerFunc, NULL) != 0) {
    fprintf(stderr, "Unable to create producer thread\n");
    exit(1);
  }
  // create three consumer
  if (pthread_create(&tid5, NULL, consumerFunc, NULL) != 0) {
    fprintf(stderr, "Unable to create consumer thread\n");
    exit(1);
  }
  if (pthread_create(&tid6, NULL, consumerFunc, NULL) != 0) {
    fprintf(stderr, "Unable to create consumer thread\n");
    exit(1);
  }
  if (pthread_create(&tid7, NULL, consumerFunc, NULL) != 0) {
    fprintf(stderr, "Unable to create consumer thread\n");
    exit(1);
  }

  // wait for created thread to exit
  pthread_join(tid1, NULL);
  pthread_join(tid2, NULL);
  pthread_join(tid3, NULL);
  pthread_join(tid4, NULL);
  pthread_join(tid5, NULL);
  pthread_join(tid6, NULL);
  pthread_join(tid7, NULL);

  return 0;
}
