#include<stdio.h>
#include<pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define PTHREAD_SUCCESS 0
#define FAIL 1
#define BUFFER_SIZE 256

sem_t first;
sem_t second;

bool isThreadsError(int errorCode){
    if(PTHREAD_SUCCESS != errorCode) {
        char message[BUFFER_SIZE];
        strerror_r(errorCode, message, sizeof message);
        fprintf(stderr, "Error: %s\n", message);
        return true;
    }
    return false;
}

void* childPrintStrings(void* arg) {
    int i;
    for(i = 0; i < 10; i++) {
        sem_wait(&second);
        printf("child : %d\n", i);
        sem_post(&first);
    }
}


int main(int argc, char* argv[]) {
    int i, status;
    pthread_t thread;

    sem_init(&first, 0, 1);
    sem_init(&second, 0, 0);
    status = pthread_create(&thread, NULL, childPrintStrings, NULL);
    if(isThreadsError(status)) {
        pthread_exit(FAIL);
    }

    for(i = 0; i < 10; i++) {
        sem_wait(&first);
        printf("parent : %d\n", i);
        sem_post(&second);
    }
    pthread_join(thread, NULL);
    sem_destroy(&first);
    sem_destroy(&second);
    return 0;
}