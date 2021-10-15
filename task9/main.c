#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <signal.h>

#define NUM_STEPS 200000000
#define SUCCESS 0
#define ERROR -1
#define BUFFER_SIZE 256

int threadsNumber;
pthread_barrier_t barrier;
pthread_mutex_t mutex;
int stop = 0;
const long smallIter = 100000;
int latestIter = 0;

struct threadInfo {
    pthread_t threadId;
    int thread;
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
    double local_pi = 0.0;
    struct threadInfo localThreadInfo = *((struct threadInfo*)threads);
    int bigIter = 0;
    while(true) {
        for(long i = localThreadInfo.thread + bigIter * smallIter * threadsNumber; i < localThreadInfo.thread + (bigIter + 1) * smallIter * threadsNumber; i += threadsNumber) {
            localThreadInfo.result += 1.0/(i*4.0 + 1.0);
            localThreadInfo.result -= 1.0/(i*4.0 + 3.0);
        }
        pthread_barrier_wait(&barrier);
        pthread_mutex_lock(&mutex);
        if ((stop == 1) && (bigIter == latestIter)) {
            pthread_mutex_unlock(&mutex);
            break;
        } else {
            bigIter++;
            if (latestIter < bigIter) {
                latestIter = bigIter;
            }
            pthread_mutex_unlock(&mutex);
        }

    }

    ((struct threadInfo *)threads)->result = localThreadInfo.result;
    pthread_exit(threads);
}

bool isThreadError(int errorCode, char *argv[]) {
    if (errorCode != SUCCESS) {
        char message[BUFFER_SIZE];
        strerror_r(errorCode, message, sizeof message);
        fprintf(stderr, "%s: Error: %s\n", argv[0], message);
        return true;
    }
    return false;
}

void sighandler(int signal) {
    stop = 1;
}

int main(int argc, char** argv) {
    if (checkInput(argc, argv) == ERROR) {
        pthread_exit((void *) ERROR);
    }
    double pi = 0.0;

    int errorCode = pthread_barrier_init(&barrier, NULL, threadsNumber);
    if (isThreadError(errorCode, argv)) {
        pthread_exit((void *) ERROR);
    }
    errorCode = pthread_mutex_init(&mutex, NULL);
    if (isThreadError(errorCode, argv)) {
        pthread_barrier_destroy(&barrier);
        pthread_exit((void *) ERROR);
    }

    signal(SIGINT, sighandler);

    struct threadInfo *threads = (struct threadInfo*)malloc(threadsNumber * sizeof(struct threadInfo));
    if (threads == NULL) {
        fprintf(stderr, "error with malloc()\n");
        pthread_barrier_destroy(&barrier);
        pthread_mutex_destroy(&mutex);
        return EXIT_FAILURE;
    }

    for (int i = 0; i < threadsNumber; ++i) {
        errorCode = pthread_create(&(threads[i].threadId), NULL, calculate, (void *)(threads + i));
        if (isThreadError(errorCode, argv)) {
            free(threads);
            pthread_barrier_destroy(&barrier);
            pthread_mutex_destroy(&mutex);
            pthread_exit((void *) ERROR);
        }
        threads[i].thread = i;
    }

    for (int i = 0; i < threadsNumber; ++i) {
        errorCode = pthread_join(threads[i].threadId, NULL);
        if (isThreadError(errorCode, argv)) {
            free(threads);
            pthread_barrier_destroy(&barrier);
            pthread_mutex_destroy(&mutex);
            pthread_exit((void *) ERROR);
        }
        pi += threads[i].result;
    }
    pi = pi * 4.0;
    printf("Pi = %.15f\n", pi);
    printf("Pi = %.15f\n", M_PI);
    free(threads);
    pthread_exit(SUCCESS);
}
