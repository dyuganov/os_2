#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define SUCCESS 0
#define REMOVE_ROUTINE 0
#define BUFFER_SIZE 256
#define SLEEP_TIME 2

void finalFunction(void *args) {
    printf("Second thread's execution is interrupted\n");
}

void *run_thread(void *args) {
    pthread_cleanup_push(finalFunction, NULL);
    char *text = "Executing thread\n";
    int len = strlen(text);

    while (1) {
        write(0, text, len);
        if (args != NULL) {
            pthread_cleanup_pop(REMOVE_ROUTINE);
        }
    }
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
        pthread_exit((void *) EXIT_FAILURE);
    }

    sleep(SLEEP_TIME);

    errCode = pthread_cancel(thread_id);
    if (isThreadError(errCode, argv)) {
        pthread_exit((void *) EXIT_FAILURE);
    }

    errCode = pthread_join(thread_id, NULL);
    if (isThreadError(errCode, argv)) {
        pthread_exit((void *) EXIT_FAILURE);
    }
    pthread_exit((void *) SUCCESS);
}