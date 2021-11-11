#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <signal.h>

#define NUM_STEPS 2000
#define SUCCESS 0
#define ERROR -1
#define BUFFER_SIZE 256

long long threadsNumber = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
bool stopFlag = false;
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

void sighandler(int signal)  {
    if (signal == SIGINT) {
        stopFlag = true;
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

void lock() {
    int errorCode = pthread_mutex_lock(&mutex);
    if (isThreadError(errorCode)) {
        pthread_exit((void *) ERROR);
    }
}

void unlock() {
    int errorCode = pthread_mutex_unlock(&mutex);
    if (isThreadError(errorCode)) {
        pthread_exit((void *) ERROR);
    }
}

void *calculate(void *threadInfo) {
    struct threadInfo *local = (struct threadInfo *)threadInfo;
    long long thread = local->thread;
    double localPi = 0.0;
    long long iter = 0;
    while (true) {
        if (stopFlag && iter != 0) {
            lock();
            if (iter >= maxIter) {
                unlock();
                break;
            }
            unlock();
        }
        else {
            lock();
            if (iter > maxIter) {
                maxIter = iter;
            }
            unlock();
            sched_yield();
        }

        long long firstStep = (iter * threadsNumber + thread) * NUM_STEPS;
        for (long long i = firstStep; i < firstStep + NUM_STEPS; i++) {
            localPi += 1.0 / (i * 4.0 + 1.0);
            localPi -= 1.0 / (i * 4.0 + 3.0);
        }
        iter++;
        fprintf(stdout, "iter = %lld", iter);
    }
    local->result = localPi;
    pthread_exit(local);
}

int main(int argc, char **argv) {
    if (checkInput(argc, argv) == ERROR) {
        pthread_exit((void *) ERROR);
    }
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = sighandler;
    sigset_t   set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    act.sa_mask = set;

    if (sigaction(SIGINT, &act, 0) == -1) {
        pthread_exit((void *) ERROR);
    }
    double pi = 0.0;
    pthread_t threadsId[threadsNumber];
    struct threadInfo threadInfo[threadsNumber];

    int errorCode;
    long long actualNumOfThreads = 0;
    for (long long i = 0; i < threadsNumber && !stopFlag; i++) {
        threadInfo[i].thread = i;
        errorCode = pthread_create(&threadsId[i], NULL, calculate, &threadInfo[i]);
        if (isThreadError(errorCode)) {
            pthread_exit((void *) ERROR);
        }
        actualNumOfThreads++;
    }
    if (actualNumOfThreads < threadsNumber) {
        stopFlag = true;
        fprintf(stderr, "Error: createThreads func\n");
    }

    for (long long i = 0; i < threadsNumber; i++) {
        struct threadInfo *local = NULL;
        errorCode = pthread_join(threadsId[i], (void **)&local);
        if (isThreadError(errorCode)) {
            pthread_exit((void *) ERROR);
        }
        if (local == NULL) {
            pthread_exit((void *) ERROR);
        }
        pi += local->result;
    }
    pi *= 4;
    printf("Pi = %.15f\n", pi);
    printf("Pi = %.15f\n", M_PI);
    pthread_exit(SUCCESS);
}
