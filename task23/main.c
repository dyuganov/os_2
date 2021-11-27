#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define SUCCESS 0
#define ERROR -1
#define BUFFER_SIZE 256
#define MAX_LEN 50
#define FIRST_CORRECT_SYMBOL 33
#define LAST_CORRECT_SYMBOL 93 //126-33

typedef struct Node {
    struct Node *next;
    struct Node *prev;
    char *value;
} ListNode;

ListNode *listHead = NULL;
int length;
char **strings = NULL;
pthread_mutex_t mutex;

bool isThreadError(int errorCode) {
    if (errorCode != SUCCESS) {
        char message[BUFFER_SIZE];
        strerror_r(errorCode, message, sizeof message);
        fprintf(stderr, "Error: %s\n", message);
        return true;
    }
    return false;
}

int checkInput(int argc, char **argv) {
     if (argc == 2){
        length = atoi(argv[1]);
        return SUCCESS;
    } else {
        fprintf(stderr, "Invalid arguments.\n");
        return ERROR;
    }
}

int createStrings() {
    strings = (char **) malloc(sizeof(char *) * length);
    if (strings == NULL) {
        fprintf(stderr, "Error with malloc.\n");
        return ERROR;
    }
    printf("==========RANDOM STRINGS==========\n");
    for (int i = 0; i < length; i++) {
        int stringSize = rand() % MAX_LEN;
        strings[i] = (char *) malloc(sizeof(char) * stringSize);
        if (strings[i] == NULL) {
            fprintf(stderr, "Error with malloc.\n");
            return ERROR;
        }
        for (int j = 0; j < stringSize; j++) {
            strings[i][j] = (rand() % (LAST_CORRECT_SYMBOL)) + FIRST_CORRECT_SYMBOL;
        }
        strings[i][stringSize - 1] = '\0';
        printf("%d : %s\n", i + 1, strings[i]);
    }
    return SUCCESS;
}

void addString(char* str) {
    if (listHead == NULL) {
        listHead = (ListNode *) malloc(sizeof(ListNode));
        listHead->prev = NULL;
        listHead->next = NULL;
        listHead->value = str;
        return;
    }
    ListNode *it = listHead;
    while (it->next != NULL) {
        it = it->next;
    }
    ListNode *newNode = (ListNode *) malloc(sizeof(ListNode));
    newNode->value = str;
    newNode->next = NULL;
    newNode->prev = it;
    it->next = newNode;
}

void* sort(void* str) {
    usleep(strlen(str) * 100000);
    pthread_mutex_lock(&mutex);
    addString(str);
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void printList(ListNode* head) {
    printf("==========SORTED LIST==========\n");
    while (head != NULL) {
        printf("length %lu : %s\n", strlen(head->value), head->value);
        head = head->next;
    }
}

void freeResources() {
    struct ListNode *tmp;
    while (listHead != NULL) {
        tmp = listHead;
        listHead = listHead->next;
        free(tmp);
    }
    for (int i = 0; i < length; i++) {
        free(strings[i]);
    }
    pthread_mutex_destroy(&mutex);
    free(strings);
}

int main(int argc, char **argv) {
    if (checkInput(argc, argv) == ERROR || createStrings() == ERROR){
        pthread_exit((void *) ERROR);
    }
    srand(time(NULL));
    pthread_mutex_init(&mutex, NULL);
    pthread_t threads[length];
    int errorCode;

    for (int i = 0; i < length; i++) {
        errorCode = pthread_create(&threads[i], NULL, sort, strings[i]);
        if(isThreadError(errorCode)) {
            pthread_exit((void *) ERROR);
        }
    }
    for (int i = 0; i < length; i++) {
        errorCode = pthread_join(threads[i], NULL);
        if (isThreadError(errorCode)) {
            pthread_exit((void *) ERROR);
        }
    }
    printList(listHead);
    freeResources();

    pthread_exit((void *) SUCCESS);
}
