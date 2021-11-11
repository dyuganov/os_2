#include<stdio.h>
#include <semaphore.h>
#include <signal.h>
#include<unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    int i;
    pid_t fork_result;
    sem_t* sem1;
    sem_t* sem2;
    char* name_sem1 = "/sem.1";
    char* name_sem2 = "/sem.2";

    //S_IRWXU | S_IRWXG | S_IRWXO == 0777 (Read, write, execute/search by owner, group and others)
    sem1 = sem_open(name_sem1, O_CREAT | O_EXCL, S_IRWXU | S_IRWXG | S_IRWXO, 1);
    sem2 = sem_open(name_sem2, O_CREAT | O_EXCL, S_IRWXU | S_IRWXG | S_IRWXO, 0);

    if (sem1 == SEM_FAILED) {
        perror("sem_open");
        exit(0);
    }
    if (sem2 == SEM_FAILED) {
        perror("sem_open");
        exit(0);
    }

    if(fork_result = fork()) {

        for(i = 0; i < 10; i++) {
            sem_wait(sem1);
            printf("child - %d\n", i);
            sem_post(sem2);
        }
    }
    else {
        if(fork_result < 0) {
            printf("fork error\n");
            exit(1);
        }

        for(i = 0; i < 10; i++) {
            sem_wait(sem2);
            printf("parent - %d\n", i);
            sem_post(sem1);
        }

    }

    sem_unlink(name_sem1);
    sem_unlink(name_sem2);
    return 0;
}