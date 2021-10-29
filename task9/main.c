#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <math.h>

#define NUM_STEPS 2000
#define SUCCESS 0
#define ERROR -1
#define BUFFER_SIZE 256

long long threadsNumber;
int stop = 0;
pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;
long long maxIter = 0;

struct threadInfo {
    long long thread;
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

void sighandler(int signal) {
    if (signal == SIGINT) {
        stop = 1;
    }
}

bool isThreadError(int errorCode) {
    if (errorCode != SUCCESS) {
        char message[BUFFER_SIZE];
        strerror_r(errorCode, message, sizeof message);
        fprintf(stderr, "Error: %s\n", message);
        return true;
    }
    return false;
}

int lock(pthread_mutex_t *mutex) {
    int errorCode = pthread_mutex_lock(mutex);
    if (isThreadError(errorCode)) {
        pthread_exit((void *) ERROR);
    }
    return errorCode;
}

int unlock(pthread_mutex_t *mutex) {
    int errorCode = pthread_mutex_unlock(mutex);
    if (isThreadError(errorCode)) {
        pthread_exit((void *) ERROR);
    }
    return errorCode;
}

void* calculate(void *threads) {
    struct threadInfo *localStruct = (struct threadInfo*) threads;
    if (localStruct == NULL) {
        fprintf(stderr, "Error with local structure in calculate func\n");
        pthread_exit(localStruct);
    }
    long long thread = localStruct->thread;
    double local_pi = 0.0;
    long long iter = 0;
    while (1) {
        if (stop == 1 && iter != 0) {
            lock(&_mutex);
            if (iter >= maxIter) {
                unlock(&_mutex);
                break;
            }
            unlock(&_mutex);
        } else {
            lock(&_mutex);
            if (iter > maxIter) {
                maxIter = iter;
            }
            unlock(&_mutex);
            sched_yield();
        }
        long long start = (iter * threadsNumber + thread) * NUM_STEPS;
        for (long long i = start; i < start + NUM_STEPS; ++i) {
            local_pi += 1.0 / (i * 4.0 + 1.0);
            local_pi -= 1.0 / (i * 4.0 + 3.0);
        }
        iter++;
    }
    localStruct->result = local_pi;
    pthread_exit(threads);
}

void delete(pthread_t *threadId,struct threadInfo *threadInfo) {
    free(threadInfo);
    free(threadId);
}

int main(int argc, char** argv) {
    if (checkInput(argc, argv) == ERROR) {
        pthread_exit((void *) ERROR);
    }
    if (signal(SIGINT, sighandler) == SIG_ERR) {
        pthread_exit((void *) ERROR);
    }
    double pi = 0.0;
    pthread_t *threadId = (pthread_t*) malloc(threadsNumber * sizeof(pthread_t));
    struct threadInfo *threadInfo = (struct threadInfo*)malloc(threadsNumber * sizeof(struct threadInfo));
    if (threadInfo == NULL || threadId == NULL) {
        fprintf(stderr, "error with malloc()\n");
        return EXIT_FAILURE;
    }

    long long errorCode, count = 0;
    for (long long i = 0; i < threadsNumber && stop == 0; ++i) {
        threadInfo[i].thread = i;
        errorCode = pthread_create(&threadId[i], NULL, calculate, &threadInfo[i]);
        if (isThreadError(errorCode)) {
            delete(threadId, threadInfo);
            pthread_exit((void *) ERROR);
        }
        count++;
    }
    if (count == 0) {
        pthread_exit((void *) ERROR);
    }
    if (count < threadsNumber) {
        stop = 1;
    }

    for (long int i = 0; i < threadsNumber; ++i) {
        struct threadInfo *local = NULL;
        errorCode = pthread_join(threadId[i], (void **) &local);
        if (isThreadError(errorCode)) {
            delete(threadId, threadInfo);
            pthread_exit((void *) ERROR);
        }
        if (local == NULL) {
            fprintf(stderr, "Error with local structure\n");
            continue;
        }
        pi += local[i].result;
    }
    pi = pi * 4.0;
    printf("Pi = %.15f\n", pi);
    printf("Pi = %.15f\n", M_PI);
    delete(threadId, threadInfo);
    pthread_exit(SUCCESS);
}
