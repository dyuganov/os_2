#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_STRING_LENGTH (256)
#define PROPORTIONAL_RATION (1000)

#define PTHREAD_SUCCESS (0)
#define BUFFER_SIZE (256)
#define LINES_NUM_CNT_ERR (-1)

#define SUCCESS (0)
#define FAIL (1)

void* threadSortJob(void* arg) {
    char* string = (char*) arg;
    int length = strnlen(string, MAX_STRING_LENGTH);

    usleep(length * PROPORTIONAL_RATION);

    printf("Length = %d -> ", length);
    printf("%s", string);

    pthread_exit(SUCCESS);
}

bool isThreadsError(int errorCode){
    if(PTHREAD_SUCCESS != errorCode) {
        char message[ERR_MSG_SIZE];
        strerror_r(errorCode, message, sizeof message);
        fprintf(stderr, "Error: %s\n", message);
        return true;
    }
    return false;
}

int getLinesNum(FILE* file){
    if(NULL == file) {
        return LINES_NUM_CNT_ERR;
    }
    int result = 0;
    char currChar = 0;
    while(!feof(file)){
        currChar = fgetc(file);
        if('\n' == currChar){
            result += 1;
        }
    }    
    fseek(file, 0, SEEK_SET);
    return result;
}

bool isGetLinesNumError(int getLinesNumResult){
    if(LINES_NUM_CNT_ERR == getLinesNumResult){
        fprintf(stderr, "Can't count strings num\n");
        return true;
    }
    return false;
}

int main (int argc, char** argv) {
    if(2 != argc) fprintf(stderr, "Wrong args num\n");

    FILE* file = fopen(argv[1], "r");
    if(NULL == file) {
        fprintf(stderr, "Can't open the file\n");
        pthread_exit(FAIL);
    }

    const int STRINGS_NUM = getLinesNum(file);
    if(isGetLinesNumError(STRINGS_NUM)){
        fclose(file);
        pthread_exit(FAIL);
    }

    char strings[STRINGS_NUM][MAX_STRING_LENGTH];
    for(int i = 0; i < STRINGS_NUM; i++) {
        fgets(strings[i], MAX_STRING_LENGTH, file);
    }
    fclose(file);

    pthread_t thread_id[STRINGS_NUM];
    for(int i = 0; i < STRINGS_NUM; i++) {
        int createResult = pthread_create(&thread_id[i], NULL, threadSortJob, strings[i]);
        if(isThreadsError(createResult)){
            pthread_exit(FAIL);
        }
    }

    for(int i = 0; i < STRINGS_NUM; i++) {
        int joinResult = pthread_join(thread_id[i], NULL);
        if(isThreadsError(joinResult)) {
            pthread_exit(FAIL);
        }
    }

    pthread_exit(SUCCESS);
}
