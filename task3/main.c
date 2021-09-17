// 3. Параметры нити
// Напишите программу, которая создает четыре нити, исполняющие одну и ту же функцию.
// Эта функция должна распечатать последовательность текстовых строк, переданных как параметр.
// Каждая из созданных нитей должна распечатать различные последовательности строк.

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NUM_OF_THREADS 4
#define SUCCESS 0
#define ERROR -1
#define BUFFER_SIZE 256

void *threadBody(void *args) {
    for (char **s = (char **)args; *s != NULL; ++s) {
        printf("%s\n", *s);
    }
    pthread_exit(SUCCESS);
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

int main(int argc, char *argv[]) {
    pthread_t treadsID[4];
    int errorCode;
    void *threadsReturn;
    char *args[][NUM_OF_THREADS] = {
            {"1 thread:: 1 line", "1 thread:: 2 line", "1 thread:: 3 line", NULL},
            {"2 thread:: 1 line", "2 thread:: 2 line", "2 thread:: 3 line", NULL},
            {"3 thread:: 1 line", "3 thread:: 2 line", "3 thread:: 3 line", NULL},
            {"4 thread:: 1 line", "4 thread:: 2 line", "4 thread:: 3 line", NULL}
    };
    for (int i = 0; i < NUM_OF_THREADS; ++i) {
        errorCode = pthread_create(&treadsID[i], NULL, threadBody, args[i]);
        if (isThreadError(errorCode, argv)) {
            pthread_exit((void *) ERROR);
        }
        errorCode = pthread_join(treadsID[i], &threadsReturn);
        if (isThreadError(errorCode, argv)) {
            pthread_exit((void *) ERROR);
        }
    }
    pthread_exit(EXIT_SUCCESS);
}
