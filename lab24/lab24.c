/*
24. Производственная линия

Разработайте имитатор производственной линии, изготавливающей винтики (widget). 
Винтик собирается из детали C и модуля, который, в свою очередь, состоит из деталей A и B. 
Для изготовления детали A требуется 1 секунда, В – две секунды, С – три секунды. 
Задержку изготовления деталей имитируйте при помощи sleep. Используйте семафоры-счетчики.
*/

#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define A_DELAY (1)
#define B_DELAY (2)
#define C_DELAY (3)

sem_t detailA, detailB, detailC, module;

void* createA (void* arg){
    while(1){
        sleep(A_DELAY);
        printf("Detail A created!\n");
        sem_post(&detailA);    
    }
}

void* createB (void* arg){
    while(1){
        sleep(B_DELAY);
        printf("Detail B created!\n");
        sem_post(&detailB);    
    }
}

void* createC (void* arg){
    while(1){
        sleep(C_DELAY);
        printf("Detail C created!\n");
        sem_post(&detailC);    
    }
}

void* createModule (void* arg){
    while(1){
        sem_wait(&detailA);
        sem_wait(&detailB);
        printf("Module created!\n");
        sem_post(&module);
    }
}

void* createWidget (){
    while(1){
        sem_wait(&module);
        sem_wait(&detailC);
        printf("Widget created!\n");
    }
}

int main(){
    pthread_t aThread;    
    pthread_t bThread;
    pthread_t cThread;
    pthread_t moduleThread;
    
    sem_init(&detailA, 0, 0);
    sem_init(&detailB, 0, 0);
    sem_init(&detailC, 0, 0);
    sem_init(&module, 0, 0);

    pthread_create(&aThread, NULL, createA, NULL);
    pthread_create(&bThread, NULL, createB, NULL);
    pthread_create(&cThread, NULL, createC, NULL);
    pthread_create(&moduleThread, NULL, createModule, NULL);
    createWidget();
}
