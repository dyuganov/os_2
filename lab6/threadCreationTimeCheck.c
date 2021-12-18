#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define SUCCESS (0)
#define FAIL (1)

#define PTHREAD_SUCCESS (0)
#define BUFFER_SIZE (256)
#define LINES_NUM_CNT_ERR (-1)

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
    if(argc != 2)  {
        fprintf(stderr, "Wrong args num\n");
        return FAIL;
    }
    const int threadsNum = atoi(argv[1]);

    pthread_t thread_id[threadsNum];
    for(int i = 0; i < threadsNum; i++) {
        int createResult = pthread_create(&thread_id[i], NULL, threadJob, NULL);
        if(isThreadsError(createResult)){
            pthread_exit(FAIL);
        }
    }

    for(int i = 0; i < threadsNum; i++) {
        int joinResult = pthread_join(thread_id[i], NULL);
        if(isThreadsError(joinResult)) {
            pthread_exit(FAIL);
        }
    }
    pthread_exit(SUCCESS);
    return SUCCESS;
}
