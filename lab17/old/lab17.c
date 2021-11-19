/*
17. Синхронизированный доступ к списку
Родительская нить программы должна считывать вводимые пользователем строки 
и помещать их в начало связанного списка. Строки длиннее 80 символов можно 
разрезать на несколько строк. При вводе пустой строки программа должна 
выдавать текущее состояние списка. Дочерняя нить пробуждается каждые пять 
секунд и сортирует список в лексикографическом порядке (используйте пузырьковую сортировку). 
Все операции над списком должны синхронизоваться при помощи мутекса.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "list.h"

#define PTHREAD_SUCCESS (0)
#define ERR_MSG_SIZE (256)
#define SUCCESS (0)
#define FAIL (1)

List* list = NULL;

bool isThreadsError(int errorCode){
    if(PTHREAD_SUCCESS != errorCode) {
        char message[ERR_MSG_SIZE];
        strerror_r(errorCode, message, sizeof message);
        fprintf(stderr, "Error: %s\n", message);
        return true;
    }
    return false;
}

void* threadJob(void* arg) {

    pthread_exit(SUCCESS);
}

int main(int argc, char** argv){
    list = createList();

    pthread_t *childThread = NULL;
    int threadCreateResult = pthread_create(childThread, NULL, threadJob, NULL);
    if(isThreadsError(threadCreateResult)){
        pthread_exit(FAIL);
    }

    int threadJoinResult = pthread_join(childThread, NULL);
    if(isThreadsError(threadJoinResult)) {
        pthread_exit(FAIL);
    }




    pthread_exit(SUCCESS);
    return SUCCESS;
}
