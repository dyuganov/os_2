#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUF_SIZE 80
#define FGETS_EMPTY_STRING "\n"

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
        exit(EXIT_FAILURE);
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
    exit(EXIT_FAILURE);
}

Node *initNode(char *str, size_t size) {
    Node *node = malloc(sizeof(Node));
    if (node == NULL) {
        free(node);
        printf("ERROR GETTING MEMORY FOR NODE\n");
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

int isEmptyString(char *buf) {
    if (!strcmp(FGETS_EMPTY_STRING, buf)) {
        printList(list);
        return 1;
    }
    return 0;
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
    while (1) {
        if (sleep(5)) {
            atExit("Error sleeping");
        }
        if (isReady(0)) return data;
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
        exit(EXIT_FAILURE);
    }

    const int N = 5;
    pthread_t threadId[N];
    for (int i = 0; i < N; ++i) {
        if (pthread_create(&threadId[i], NULL, sort, NULL)) {
            atExit("Error creating thread");
        }
    }

    getStrings();

    for (int i = 0; i < N; ++i) {
        if (pthread_join(threadId[i], NULL)) {
            atExit("Error waiting thread");
        }
    }

    destroyMutex();
    freeList(list);
}
