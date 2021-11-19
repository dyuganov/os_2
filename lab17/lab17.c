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
            exit(EXIT_FAILURE);
        }
        tmpNode = iter->next;
        free(iter);
    }
}

void atExit(char *errorMsg) {
    perror(errorMsg);
    freeList(head);
    exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
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
    printf("\n\n------------------LIST:\n");
    for (Node *iter = head->next; iter; iter = iter->next) {
        lockMutex(&(iter->mutex));
        unlockMutex(&(prev->mutex));
        printf("%s\n", iter->string);
        prev = iter;
    }
    unlockMutex(&(prev->mutex));
    printf("----------------------:\n");

}

Node *initList() {
    Node *head = malloc(sizeof(Node));
    head->string = NULL;
    head->next = NULL;
    if (pthread_mutex_init(&(head->mutex), NULL) < 0) {
        perror("Error mutex init");
        exit(EXIT_FAILURE);
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
    int true = 1;
    int sleepingTime = 5;
    while (true) {
        printf("Sorting starts, st = %d\n", sleepingTime);
        if (isReady(0)) return data;
        if (sleep(sleepingTime)) {
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
                    usleep(100);
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

void createThread() {
    pthread_t threadId, threadId2;

    if (pthread_create(&threadId, NULL, sort, NULL)) {
        atExit("Error creating thread");
    }
/*
    if(pthread_create(&threadId2, NULL, sort, NULL)){
	atExit("Error creating second thread");
    }
*/

    getStrings();

    if (pthread_join(threadId, NULL)) {
        atExit("Error waiting thread");
    }

    freeList(head);
}

int main() {
    createThread();
}
