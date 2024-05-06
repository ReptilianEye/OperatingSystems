#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>
#include <signal.h>
#include <semaphore.h>
#define SHARED_MEMORY_NAME "shared_memory"

int finish = 0;

void handler(int sig)
{
    finish = 1;
}

typedef struct
{
    sem_t semaphore;
    char buffer[100];
    size_t buffer_size;
} printer_t;

typedef struct
{
    printer_t printers[10];
    size_t printers_n;
} memory_map_t;

void random_string(char *s, const int len)
{
    for (int i = 0; i < len; ++i)
        s[i] = 'a' + rand() % 26;
    s[len] = '\0';
}
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <number of users>\n", argv[0]);
        return 1;
    }

    long users_n = strtol(argv[1], NULL, 10);

    signal(SIGINT, handler);

    int mem_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (mem_fd == -1)
    {
        perror("shm_open");
        return 1;
    }

    memory_map_t *memory_map = mmap(NULL, sizeof(memory_map_t), PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);

    if (memory_map == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    char user_in_string[100] = {0};

    for (int i = 0; i < users_n; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            while (!finish)
            {
                random_string(user_in_string, 10);

                int idle_printer_index = -1;
                for (int j = 0; j < memory_map->printers_n; j++)
                {
                    int tmp;
                    sem_getvalue(&memory_map->printers[j].semaphore, &tmp);
                    if (tmp > 1)
                    {
                        idle_printer_index = j;
                        break;
                    }
                }
                if (idle_printer_index == -1)
                    idle_printer_index = rand() % memory_map->printers_n;

                if (sem_wait(&memory_map->printers[idle_printer_index].semaphore) < 0)
                {
                    perror("sem_wait");
                    return 1;
                }

                memory_map->printers[idle_printer_index].buffer_size = strlen(user_in_string);
                memcpy(memory_map->printers[idle_printer_index].buffer, user_in_string, memory_map->printers[idle_printer_index].buffer_size);

                printf("User %d: sent %s to printer %d\n", i, user_in_string, idle_printer_index);
                // fflush(stdout);
                sleep(rand() % 5 + 1);
            }
            exit(0);
        }
    }
    while (wait(NULL) > 0)
    {
    }

    if (munmap(memory_map, sizeof(memory_map_t)) < 0)
    {
        perror("munmap");
        return 1;
    }
}
