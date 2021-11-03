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
#include <stdbool.h>

#define EXIT_FAIL (-1);

#define A_DELAY (1)
#define B_DELAY (2)
#define C_DELAY (3)

#define WIDGET_NUM (3)
#define MODULE_NUM (WIDGET_NUM)
#define DETAIL_A_NUM (WIDGET_NUM)
#define DETAIL_B_NUM (WIDGET_NUM)
#define DETAIL_C_NUM (WIDGET_NUM)

#define SEM_ERROR_CODE (-1)
#define THR_SUCCESS (0)
#define THR_ERR_BUF_SIZE (256)

sem_t detailA, detailB, detailC, module;

void createDetail(const long delailsCnt, const long creationDelay, char** message, sem_t* semToPost){
  long cnt = 0;
  while(1){
      if(delailsCnt <= cnt){
          pthread_exit(0);
      }
      sleep(creationDelay);
      printf("%s", message);
      sem_post(semToPost);
      ++cnt;
  }
}

void* createA (void* arg){
    createDetail(DETAIL_A_NUM, A_DELAY, "Detail A created!\n", &detailA);
}

void* createB (void* arg){
    createDetail(DETAIL_B_NUM, B_DELAY, "Detail B created!\n", &detailB);
}

void* createC (void* arg){
    createDetail(DETAIL_C_NUM, C_DELAY, "Detail C created!\n", &detailC);
}

void* createModule (void* arg){
    long cnt = 0;
    while(1){
        if(MODULE_NUM <= cnt){
            pthread_exit(0);
        }
        sem_wait(&detailA);
        sem_wait(&detailB);
        printf("Module created!\n");
        sem_post(&module);
        ++cnt;
    }
}

void* createWidget (){
    long cnt = 0;
    while(1){
        if(WIDGET_NUM <= cnt){
            pthread_exit(0);
        }
        sem_wait(&module);
        sem_wait(&detailC);
        printf("Widget created!\n");
        ++cnt;
    }
}

bool isThreadError(int errorCode) {
    if (errorCode != THR_SUCCESS) {
        char message[THR_ERR_BUF_SIZE];
        strerror_r(errorCode, message, sizeof message);
        fprintf(stderr, "Error: %s\n", message);
        return true;
    }
    return false;
}

bool isSemError(int semResult){
    if(SEM_ERROR_CODE == semResult){
        perror("sem_ func error");
        return true;
    }
    return false;
}

int main(){
    pthread_t aThread;
    pthread_t bThread;
    pthread_t cThread;
    pthread_t moduleThread;

    int semResult = sem_init(&detailA, 0, 0);
    if(isSemError(semResult)) return EXIT_FAIL;
    semResult = sem_init(&detailB, 0, 0);
    if(isSemError(semResult)) return EXIT_FAIL;
    semResult = sem_init(&detailC, 0, 0);
    if(isSemError(semResult)) return EXIT_FAIL;
    semResult = sem_init(&module, 0, 0);
    if(isSemError(semResult)) return EXIT_FAIL;

    int pthreadResult = pthread_create(&aThread, NULL, createA, NULL);
    if(isThreadError(pthreadResult)) return EXIT_FAIL;
    pthreadResult = pthread_create(&bThread, NULL, createB, NULL);
    if(isThreadError(pthreadResult)) return EXIT_FAIL;
    pthreadResult = pthread_create(&cThread, NULL, createC, NULL);
    if(isThreadError(pthreadResult)) return EXIT_FAIL;
    pthreadResult = pthread_create(&moduleThread, NULL, createModule, NULL);
    if(isThreadError(pthreadResult)) return EXIT_FAIL;

    createWidget();

    pthreadResult = pthread_join(aThread, NULL);
    if(isThreadError(pthreadResult)) return EXIT_FAIL;
    pthreadResult = pthread_join(bThread, NULL);
    if(isThreadError(pthreadResult)) return EXIT_FAIL;
    pthreadResult = pthread_join(cThread, NULL);
    if(isThreadError(pthreadResult)) return EXIT_FAIL;
    pthreadResult = pthread_join(moduleThread, NULL);
    if(isThreadError(pthreadResult)) return EXIT_FAIL;
}
