#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t reindeer_back_mutex = PTHREAD_MUTEX_INITIALIZER;
int reindeer_back = 0;

pthread_mutex_t reindeers[9];

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;

pthread_t santa_thread;
pthread_t reindeer_threads[9];

void *santa()
{
    for (int i = 0; i < 4; i++)
    {
        pthread_cond_wait(&santa_cond, &santa_mutex);
        printf("Mikołaj: Budzę się!\n");

        printf("Mikołaj: Rozdaję prezenty!\n");

        sleep(rand() % 3 + 3);

        for (int i = 0; i < 9; i++)
        {
            pthread_mutex_unlock(&reindeers[i]);
        }

        printf("Mikołaj: Zabawki dostarczone, zasypiam!\n");
    }

    for (int i = 0; i < 9; i++)
    {
        pthread_cancel(reindeer_threads[i]);
    }

    pthread_exit(NULL);
}

void *reindeer(void *args)
{
    int reindeer_id = *((int *)args);

    pthread_mutex_lock(&reindeers[reindeer_id]);
    while (1)
    {
        sleep(rand() % 6 + 5);

        pthread_mutex_lock(&reindeer_back_mutex);
        reindeer_back++;

        printf("Renifer %d: Czekam na Mikołaja, liczba reniferów: %d\n", reindeer_id, reindeer_back);
        if (reindeer_back == 9)
        {
            printf("Renifer %d: Wybudzam Mikołaja!\n", reindeer_id);
            pthread_cond_signal(&santa_cond);
            reindeer_back = 0;
        }

        pthread_mutex_unlock(&reindeer_back_mutex);

        pthread_mutex_lock(&reindeers[reindeer_id]);

        printf("Renifer %d: Dostarczyłem zabawki, lece na wakacje!\n", reindeer_id);
    }
    pthread_exit(NULL);
}

int main()
{

    for (int i = 0; i < 9; i++)
    {
        pthread_mutex_init(&reindeers[i], NULL);
    }
    int reindeers_ids[9];
    pthread_create(&santa_thread, NULL, santa, NULL);
    for (int i = 0; i < 9; i++)
    {
        reindeers_ids[i] = i;
        pthread_create(&reindeer_threads[i], NULL, reindeer, &reindeers_ids[i]);
    }

    pthread_join(santa_thread, NULL);
    for (int i = 0; i < 9; i++)
    {
        pthread_join(reindeer_threads[i], NULL);
    }

    return 0;
}