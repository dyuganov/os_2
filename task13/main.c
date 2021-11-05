#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define PARENT 1
#define CHILD 2
#define NUM_OF_STRING 10
#define SUCCESS 0
#define ERROR 1
#define BUFFER_SIZE 256

// 0 == "nobody's (common) mutex"
// 1 == "parent's mutex"
// 2 == "child's mutex"

pthread_t thread;
pthread_mutex_t mutex;
pthread_cond_t cond;

int currentThread = PARENT;

bool isThreadError(int errorCode) {
    if (errorCode != SUCCESS) {
        char message[BUFFER_SIZE];
        strerror_r(errorCode, message, sizeof message);
        fprintf(stderr, "Error: %s\n", message);
        return true;
    }
    return false;
}

void destroyMutexes() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
}

void lockMutex() {
    int errorCode = pthread_mutex_lock(&mutex);
    if (isThreadError(errorCode)) {
        destroyMutexes();
        pthread_exit((void *) ERROR);
    }
}

void unlockMutex() {
    int errorCode = pthread_mutex_unlock(&mutex);
    if (isThreadError(errorCode)) {
        destroyMutexes();
        pthread_exit((void *) ERROR);
    }
}

void waitCond() {
    int errorCode = pthread_cond_wait(&cond, &mutex);
    if (isThreadError(errorCode)) {
        destroyMutexes();
        pthread_exit((void *) ERROR);
    }
}

void condSignal() {
    int errorCode = pthread_cond_signal(&cond);
    if (isThreadError(errorCode)) {
        destroyMutexes();
        pthread_exit((void *) ERROR);
    }
}

void* childPrintStrings() {
    lockMutex();
    for (int i = 0; i < NUM_OF_STRING; i++) {
        while (currentThread != CHILD) {
            waitCond();
        }
        printf("CHILD THREAD: %d\n", i + 1);
        currentThread = PARENT;
        condSignal();
    }
    unlockMutex();
}

void startChildThread() {
    int errorCode = pthread_create(&thread, NULL, childPrintStrings, NULL);
    if (isThreadError(errorCode)) {
        destroyMutexes();
        pthread_exit((void *) ERROR);
    }
}

void parentPrintStrings() {
    lockMutex();
    for (int i = 0; i < NUM_OF_STRING; i++) {
        while (currentThread != PARENT) {
            waitCond();
        }
        printf("PARENT THREAD: %d\n", i + 1);
        currentThread = CHILD;
        condSignal();
    }
    unlockMutex();
}

int main(int argc, char *argv[]) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    int errorCode = pthread_cond_init(&cond, NULL);
    if (isThreadError(errorCode)) {
        pthread_exit((void *) ERROR);
    }

    startChildThread();
    parentPrintStrings();

    destroyMutexes();
    pthread_exit(SUCCESS);
}