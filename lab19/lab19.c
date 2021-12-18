/*
19. Синхронизированный доступ к списку 3

Модифицируйте программу упр. 18 так, чтобы дочерняя нить засыпала на одну секунду
между исполнениями каждого шага сортировки (между перестановками записей в списке).
При этом можно будет наблюдать процесс сортировки по шагам.
*/
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
    pthread_mutex_t mutex;
} Node;

Node *head;
int size = 0;


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

void freeList(Node *head) {
    if (head == NULL) {
        return;
    }
    Node *tmpNode;
    for (Node *iter = head; iter; iter = tmpNode) {
        if (pthread_mutex_destroy(&(iter->mutex)) < 0) {
            perror("Error destroying mutex");
            pthread_exit(EXIT_FAILURE);
        }
        tmpNode = iter->next;
        free(iter);
    }
}

void atExit(char *errorMsg) {
    perror(errorMsg);
    freeList(head);
    pthread_exit(EXIT_FAILURE);
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

Node *initNode(char *str, size_t size) {
    Node *node = malloc(sizeof(Node));
    if (node == NULL) {
        free(node);
        printf("ERROR GETTING MEMORY FOR NODE\n");
        return NULL;
    }
    if (pthread_mutex_init(&(node->mutex), NULL) < 0) {
        perror("Error creating mutex");
        pthread_exit(EXIT_FAILURE);
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

void pushFront(Node *head, char *str) {
    if (head == NULL) {
        return;
    }
    Node *newNode = initNode(str, strlen(str));
    if (newNode == NULL) {
        return;
    }
    lockMutex(&(head->mutex));
    newNode->next = head->next;
    head->next = newNode;
    size++;
    unlockMutex(&(head->mutex));
}

void printList(Node *head) {
    if (head == NULL) {
        return;
    }
    lockMutex(&(head->mutex));
    Node *prev = head;
    printf("\n-----LIST----\n");
    for (Node *iter = head->next; iter; iter = iter->next) {
        lockMutex(&(iter->mutex));
        unlockMutex(&(prev->mutex));
        printf("%s", iter->string);
        prev = iter;
    }
    unlockMutex(&(prev->mutex));
    printf("-------------\n");

}

Node *initList() {
    Node *head = malloc(sizeof(Node));
    head->string = NULL;
    head->next = NULL;
    if (pthread_mutex_init(&(head->mutex), NULL) < 0) {
        perror("Error mutex init");
        pthread_exit(EXIT_FAILURE);
    }
    return head;
}

int isEmptyString(char *buf) {
    if (!strcmp(FGETS_EMPTY_STRING, buf)) {
        printList(head);
        return 1;
    }
    return 0;
}

void getStrings() {
    char buf[BUF_SIZE + 1];
    head = initList();
    while (fgets(buf, BUF_SIZE + 1, stdin)) {
        if (!isEmptyString(buf)) {
            pushFront(head, buf);
        }
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
    const int SLEEP_TIME = 5;
    while (1) {
        //printf("Sorting starts, st = %d\n", SLEEP_TIME);
        if (isReady(0)) return data;
        if (sleep(SLEEP_TIME)) {
            atExit("Error sleeping");
        }
        Node *prev = head;
        int i = 0, j = 0;
        for (Node *node = head->next; node; node = node->next, ++i, j = 0) {
            prev = head;
            lockMutex(&(head->mutex));
            lockMutex(&(head->next->mutex));
            for (Node *innerNode = head->next; innerNode->next; innerNode = innerNode->next, ++j) {
                lockMutex(&(innerNode->next->mutex));
                if (compare(innerNode->next->string, innerNode->string) < 0) {
                    //sleep(1);
                    usleep(1000);
                    swap(&(innerNode->next->string), &(innerNode->string));
                }
                unlockMutex(&(prev->mutex));
                prev = innerNode;
            }
            unlockMutex(&(prev->mutex));
            unlockMutex(&(prev->next->mutex));
        }
    }
    return data;
}

int main() {
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

    freeList(head);
    pthread_exit(EXIT_SUCCESS);
}
