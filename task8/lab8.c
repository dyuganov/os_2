#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define NUM_STEPS 200000000
#define SUCCESS 0
#define ERROR -1
#define BUFFER_SIZE 256


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
long threadsNumber;

struct threadInfo {
    pthread_t threadId ;
    int thread ;
    double result;
};

int checkInput(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <expected number (number of threads)>\n", argv[0]);
        return ERROR;
    }
    threadsNumber = atol(argv[1]);
    if (threadsNumber < 1) {
        fprintf(stderr, "%d is incorrect number of threads\n", threadsNumber);
        return ERROR;
    }
    return SUCCESS;
}

void* calculate(void *threads) {
    double local_pi = 0.0;
    pthread_mutex_lock(&mutex);
    long i = ((struct threadInfo *)threads)->thread;
    for (; i < NUM_STEPS ; i += threadsNumber) {
        local_pi += 1.0 / (i * 4.0 + 1.0);
        local_pi -= 1.0 / (i * 4.0 + 3.0);
    }
    ((struct threadInfo *)threads)->result = local_pi;
    pthread_mutex_unlock(&mutex);
    return threads;
}

bool isThreadError(int errorCode, char *argv[], struct threadInfo *threads) {
    if (errorCode != SUCCESS) {
        char message[BUFFER_SIZE];
        strerror_r(errorCode, message, sizeof message);
        free(threads);
        fprintf(stderr, "%s: Error: %s\n", argv[0], message);
        return true;
    }
    return false;
}

int main(int argc, char** argv) {
    if (checkInput(argc, argv) == ERROR) {
        pthread_exit((void *) ERROR);
    }
    struct threadInfo *threads = (struct threadInfo*)malloc(threadsNumber * sizeof(struct threadInfo));


    if (threads == NULL) {
        fprintf(stderr, "error with malloc()\n");
        return EXIT_FAILURE;
    }

    for ( int i = 0 ; i < threadsNumber; i++) {
        threads[i].result = 0;
        threads[i].threadId = 0;
        threads[i].thread = 0;
    }

    int errorCode;
    for (int i = 0; i < threadsNumber; ++i) {
        threads[i].thread = i;
        errorCode = pthread_create(&(threads[i].threadId), NULL, calculate, (void *)(threads + i));
        if (isThreadError(errorCode, argv, threads)) {
            pthread_exit((void *) ERROR);
        }
    }

    double pi = 0.0;
    for (int i = 0; i < threadsNumber; ++i) {
        errorCode = pthread_join(threads[i].threadId, NULL);
        if (isThreadError(errorCode, argv, threads)) {
            pthread_exit((void *) ERROR);
        }
        pi += threads[i].result;
    }
    pi = pi * 4.0;
    printf("Pi = %.13f\n", pi);
    free(threads);
    pthread_exit(SUCCESS);
}
