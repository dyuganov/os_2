#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define PHILO 5
#define DELAY 30000
#define FOOD 50

pthread_mutex_t forks[PHILO];
pthread_t phils[PHILO];

void *philosopher(void *id);

int food_on_table();

void get_fork(int, int, char *, int);

void down_forks(int, int);

pthread_mutex_t foodlock;
int sleep_seconds = 0;


pthread_mutex_t left;

int main(int argn, char **argv) {
    int i;

    if (argn == 2)
        sleep_seconds = atoi(argv[1]);

    //обавили мьютекс и его блокировку на левую вилку
    pthread_mutex_init(&left, NULL);

    pthread_mutex_init(&foodlock, NULL);
    for (i = 0; i < PHILO; i++)
        pthread_mutex_init(&forks[i], NULL);
    for (i = 0; i < PHILO; i++)
        pthread_create(&phils[i], NULL, philosopher, (void *) i);
    for (i = 0; i < PHILO; i++)
        pthread_join(phils[i], NULL);
    return 0;
}

void *philosopher(void *num) {
    int id;
    int left_fork, right_fork, f;
    id = (int) num;

    left_fork = id;
    right_fork = id + 1;

    /* Wrap around the forks. */
    if (left_fork == PHILO)
        left_fork = 0;

    while (f = food_on_table()) {
        /* Thanks to philosophers #1 who would like to
         * take a nap before picking up the forks, the other
         * philosophers may be able to eat their dishes and
         * not deadlock.
         */
        if (id == 1)
            sleep(sleep_seconds);

        get_fork(id, left_fork, "left", 0);
        get_fork(id, right_fork, "right", 1);

        usleep(DELAY * (FOOD - f + 1));
        down_forks(left_fork, right_fork);
    }
    return (NULL);
}

int food_on_table() {
    static int food = FOOD;
    int myfood;
    pthread_mutex_lock(&foodlock);
    if (food > 0) {
        food--;
    }
    myfood = food;
    pthread_mutex_unlock(&foodlock);
    return myfood;
}

void get_fork(int phil, int fork, char *hand, int is_left) {

    if (is_left == 0)
        pthread_mutex_lock(&left);

    pthread_mutex_lock(&forks[fork]);

    if (is_left == 0)
        pthread_mutex_unlock(&left);

    printf("Philosopher %d: got %s fork %d\n", phil, hand, fork);
}

void down_forks(int f1, int f2) {
    pthread_mutex_unlock(&forks[f1]);
    pthread_mutex_unlock(&forks[f2]);
}
