#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

//2. Ожидание нити
//
//Модифицируйте программу упр. 1 так, чтобы вывод родительской
// нити производился после завершения дочерней. Используйте pthread_join.

#define SUCCESS 0
#define ERROR 1
#define BUFFER_SIZE 256

void *thread_body(void *args) {
    for (int i = 0; i < 10; i++) {
        printf("Child\n");
    }
    return NULL;
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
    pthread_t thread;
    int errorCode = pthread_create(&thread, NULL, thread_body, NULL); //создание нити
    // 0 при успешном завершении, при ошибке - код ошибки
    // аргументы:
    // 1. указатель на поток - переменная, куда в случае удачного завершения сохраняет id потока
    // 2. атрибуты потока - NULL -> атрибуты по умолчанию
    // 3. та ф-я, которая будет выполняться в новом потоке
    // 4. аргументы для этой самой ф-и
    if (isThreadError(errorCode, argv)) {
        exit(ERROR);
    }
    errorCode = pthread_join(thread, NULL); // ожидание завершения потока -- блокирует вызывающий поток, пока указанный поток не завершится
    // аргументы:
    // 1. идентификатор нити
    // 2. указатель на переменную, в которой будет размещаться значение кода возврата. NULL -- код возврата игнорируется
    if (isThreadError(errorCode, argv)) {
        exit(ERROR);
    }
    for (int i = 0; i < 10; i++) {
        printf("Parent\n");
    }
    pthread_exit(NULL);
}
