#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define STRING_NUM (20)
#define MAX_STRING_LENGTH (256)
#define PROPORTIONAL_RATION (100)

#define PTHREAD_SUCCESS (0)
#define BUFFER_SIZE (256)

pthread_t thread_id[STRING_NUM];

void* threadJob(void* arg) {
    char* string = (char*) arg;
    int length = strnlen(string, MAX_STRING_LENGTH);

    usleep(length * PROPORTIONAL_RATION * PROPORTIONAL_RATION);

    printf("Length = %d -> ",length);
    printf("%s", string);

    pthread_exit(0);
}

bool isThreadsError(int errorCode){
    if(PTHREAD_SUCCESS != errorCode) {
        char message[BUFFER_SIZE];
        strerror_r(errorCode, message, sizeof message);
        fprintf(stderr, "Error: %s\n", message);
        return true;
    }
    return false;
}

int main (int argc, char** argv) {
    if(2 > argc) fprintf(stderr, "Wrong args num\n");

    FILE* file = fopen(argv[1], "r");
    if(NULL == file) {
        fprintf(stderr, "Can't open the file\n");
        exit(1);
    }

    char strings[STRING_NUM][MAX_STRING_LENGTH];
    for(int i = 0; i < STRING_NUM; i++) {
        fgets(strings[i], MAX_STRING_LENGTH, file);
    }
    fclose(file);

    for(int i = 0; i < STRING_NUM; i++) {
        int createResult = pthread_create(&thread_id[i], NULL, threadJob, strings[i]);
        if(isThreadsError(createResult)){
            exit(1);
        }
    }

    for(int i = 0; i < STRING_NUM; i++) {
        int joinResult = pthread_join(thread_id[i], NULL);
        if(isThreadsError(joinResult)) {
            exit(1);
        }
    }

    return 0;
}
