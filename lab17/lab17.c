/*
17. Синхронизированный доступ к списку
Родительская нить программы должна считывать вводимые пользователем строки
и помещать их в начало связанного списка. Строки длиннее 80 символов можно
разрезать на несколько строк. При вводе пустой строки программа должна
выдавать текущее состояние списка. Дочерняя нить пробуждается каждые пять
секунд и сортирует список в лексикографическом порядке (используйте пузырьковую сортировку).
Все операции над списком должны синхронизоваться при помощи мутекса.
*/

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define BUF_SIZE 80
#define FGETS_EMPTY_STRING "\n"

#define SLEEP_TIME (5)

typedef struct Node {
    char *string;
    struct Node *next;
} Node;

typedef struct List {
    Node *head;
    int size;
} List;

pthread_mutex_t mutex;
List *list;

int isReady(int fd) {
    fd_set fdset;
    struct timeval timeout;
    int ret;
    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);
    timeout.tv_sec = 0;
    timeout.tv_usec = 1;
    return select(fd + 1, &fdset, NULL, NULL, &timeout) == 1 ? 1 : 0;
}

void destroyMutex() {
    if (pthread_mutex_destroy(&mutex) < 0) {
        perror("Error destoying mutex");
        pthread_exit(EXIT_FAILURE);
    }
}

void freeList(List *list) {
    if (list == NULL) {
        return;
    }
    Node *tmpNode;
    for (Node *iter = list->head; iter; iter = tmpNode) {
        tmpNode = iter->next;
        free(iter);
    }
    free(list);
}

void atExit(char *errorMsg) {
    perror(errorMsg);
    destroyMutex();
    freeList(list);
    pthread_exit(EXIT_FAILURE);
}

Node *initNode(char *str, size_t size) {
    Node *node = malloc(sizeof(Node));
    if (node == NULL) {
        free(node);
        perror("ERROR GETTING MEMORY FOR NODE\n");
        return NULL;
    }
    node->next = NULL;
    node->string = malloc(size + 1);
    if (strcpy(node->string, str) == NULL) {
        free(node->string);
        free(node);
        perror("Error creating node for input string");
        return NULL;
    }
    return node;
}

void pushFront(List *list, char *str) {
    if (list == NULL) {
        return;
    }
    Node *newNode = initNode(str, strlen(str));
    if (newNode == NULL) {
        return;
    }
    newNode->next = list->head;
    list->head = newNode;
    (list->size)++;
}

void printList(List *list) {
    if (list == NULL) {
        return;
    }
    printf("\n-----LIST----\n");
    for (Node *iter = list->head; iter; iter = iter->next) {
        printf("%s", iter->string);
    }
    printf("-------------\n");
}

List *initList() {
    List *list = malloc(sizeof(List));
    list->head = NULL;
    list->size = 0;
    return list;
}

void lockMutex(pthread_mutex_t *mutex) {
    if (pthread_mutex_lock(mutex)) {
        atExit("Error locking mutex");
    }
}

void unlockMutex(pthread_mutex_t *mutex) {
    if (pthread_mutex_unlock(mutex)) {
        atExit("Error unlocking mutex");
    }
}

bool isEmptyString(char *buf) {
    if (!strcmp(FGETS_EMPTY_STRING, buf)) {
        printList(list);
        return true;
    }
    return false;
}

void getStrings() {
    char buf[BUF_SIZE + 1];
    list = initList();
    while (fgets(buf, BUF_SIZE + 1, stdin)) {
        lockMutex(&mutex);
        if (!isEmptyString(buf)) {
            pushFront(list, buf);
        }
        unlockMutex(&mutex);
        fflush(stdout);
    }
}

void swap(char **left, char **right) {
    char *tmp = *left;
    *left = *right;
    *right = tmp;
}

int compare(char *left, char *right) {
    int leftLen = strlen(left), rightLen = strlen(right);
    int minLength = (leftLen > rightLen) ? rightLen : leftLen;
    int maxLength = (leftLen < rightLen) ? rightLen : leftLen;
    for (int i = 0; i < minLength; ++i) {
        if (left[i] != right[i]) {
            return 2 * (left[i] > right[i]) - 1;
        }
    }
    if (!(maxLength - minLength)) {
        return 0;
    }
    return 2 * (maxLength == strlen(left)) - 1;
}

void *sort(void *data) {
    while (true) {
        if (sleep(SLEEP_TIME)) {
            atExit("Error sleeping");
        }
        if (isReady(stdin)) return data;
        printf("Starting sort\n");
        lockMutex(&mutex);
        int i = 0, j = 0;
        for (Node *node = list->head; node; node = node->next, ++i, j = 0) {
            for (Node *innerNode = list->head; j < list->size - i - 1; innerNode = innerNode->next, ++j) {
                if (compare(innerNode->next->string, innerNode->string) < 0) {
                    swap(&(innerNode->next->string), &(innerNode->string));
                }
            }
        }
        unlockMutex(&mutex);
    }
    return data;
}

int main() {
    if (pthread_mutex_init(&mutex, NULL) < 0) {
        perror("Error creating mutex");
        pthread_exit(EXIT_FAILURE);
    }
    pthread_t threadId;
    if (pthread_create(&threadId, NULL, sort, NULL)) {
        atExit("Error creating thread");
    }
    getStrings();
    if (pthread_join(threadId, NULL)) {
        atExit("Error waiting thread");
    }
    destroyMutex();
    freeList(list);
    pthread_exit(EXIT_SUCCESS);
}
