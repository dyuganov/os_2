#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define SUCCESS 0
#define ERROR 1
#define BUFFER_SIZE 256

void *run_thread(void *args) {
    while (1) {
        printf("Executing thread\n");
    }
    return 0;
}

bool isThreadError(int errorCode, char *argv[]) {
    if (errorCode != SUCCESS) {
        char message[BUFFER_SIZE];
        strerror_r(errorCode, message, sizeof message); //возвращают строку в буфере message, описывающую ошибку
        fprintf(stderr, "%s: Error: %s\n", argv[0], message);
        return true;
    }
    return false;
}

int main(int argc, char *argv[]) {
    pthread_t thread_id;
    int errCode = pthread_create(&thread_id, NULL, run_thread, NULL);

    if (isThreadError(errCode, argv)) {
        exit(ERROR);
    }

    sleep(2);
    errCode = pthread_cancel(thread_id);
    if (isThreadError(errCode, argv)) {
        exit(ERROR);
    }

    errCode = pthread_join(thread_id, NULL);
    if (isThreadError(errCode, argv)) {
        exit(ERROR);
    }

    return 0;
}
