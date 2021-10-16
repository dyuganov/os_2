#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define NUM_STEPS 10000000
#define SUCCESS 0
#define ERROR -1
#define BUFFER_SIZE 256

long int threadsNumber;
long int iterForOneThread;

struct threadInfo {
    long int thread;
    double result;
};

int checkInput(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <expected number (number of threads)>\n", argv[0]);
        return ERROR;
    }
    threadsNumber = atoi(argv[1]);
    if (threadsNumber < 1) {
        fprintf(stderr, "%d is incorrect number of threads\n", threadsNumber);
        return ERROR;
    }
    return SUCCESS;
}

void* calculate(void *threads) {
    struct threadInfo localStruct = *((struct threadInfo*) threads);
    for (long int i = localStruct.thread; i < (iterForOneThread + localStruct.thread) ; ++i) {
        localStruct.result += 1.0 / (i * 4.0 + 1.0);
        localStruct.result -= 1.0 / (i * 4.0 + 3.0);
    }
    ((struct threadInfo *)threads)->result = localStruct.result;
    pthread_exit(threads);
}

bool isThreadError(int errorCode, char *argv[], struct threadInfo *threads, pthread_t *threadId) {
    if (errorCode != SUCCESS) {
        char message[BUFFER_SIZE];
        strerror_r(errorCode, message, sizeof message);
        free(threads);
        free(threadId);
        fprintf(stderr, "%s: Error: %s\n", argv[0], message);
        return true;
    }
    return false;
}

int main(int argc, char** argv) {
    if (checkInput(argc, argv) == ERROR) {
        pthread_exit((void *) ERROR);
    }
    double pi = 0.0;
    pthread_t *threadId = (pthread_t*) malloc(threadsNumber * sizeof(pthread_t));
    struct threadInfo *threads = (struct threadInfo*)malloc(threadsNumber * sizeof(struct threadInfo));
    if (threads == NULL || threadId == NULL) {
        fprintf(stderr, "error with malloc()\n");
        return EXIT_FAILURE;
    }
    iterForOneThread = NUM_STEPS / threadsNumber;

    int errorCode;
    for (long int i = 0; i < threadsNumber; ++i) {
        threads[i].thread = i * iterForOneThread;
        threads[i].result = 0;
        errorCode = pthread_create(&(threadId[i]), NULL, calculate, &(threads[i]));
        if (isThreadError(errorCode, argv, threads, threadId)) {
            pthread_exit((void *) ERROR);
        }
    }

    for (long int i = 0; i < threadsNumber; ++i) {
        struct threadInfo *localResult;
        errorCode = pthread_join(threadId[i], (void **) &localResult);
        if (isThreadError(errorCode, argv, threads, threadId)) {
            pthread_exit((void *) ERROR);
        }
        pi += localResult[i].result;
    }
    pi = pi * 4.0;
    printf("Pi = %.15f\n", pi);
    printf("Pi = %.15f\n", M_PI);
    free(threads);
    free(threadId);
    pthread_exit(SUCCESS);
}
