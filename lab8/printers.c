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

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        printf("Usage: %s <number of printers>\n", argv[0]);
        return 1;
    }

    long printers_n = strtol(argv[1], NULL, 10);

    signal(SIGINT, handler);

    int mem_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (mem_fd == -1)
    {
        perror("shm_open");
        return 1;
    }

    if (ftruncate(mem_fd, sizeof(memory_map_t)) < 0)
    {
        perror("ftruncate");
        return 1;
    }

    memory_map_t *memory_map = mmap(NULL, sizeof(memory_map_t), PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 0);

    if (memory_map == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    memset(memory_map, 0, sizeof(memory_map_t));
    memory_map->printers_n = printers_n;

    for (int i = 0; i < printers_n; i++)
    {
        sem_init(&memory_map->printers[i].semaphore, 1, 1);
        pid_t pid = fork();
        if (pid == 0)
        {
            while (!finish)
            {
                int val;
                sem_getvalue(&memory_map->printers[i].semaphore, &val);
                if (val == 1)
                {

                    for (int j = 0; j < memory_map->printers[i].buffer_size; j++)
                    {
                        printf("%c", memory_map->printers[i].buffer[j]);
                        sleep(1);
                    }
                    // printf("drukarka %d", i);
                    printf("\n");
                    fflush(stdout);

                    sem_post(&memory_map->printers[i].semaphore);
                }
            }
            exit(0);
        }
    }
    while (wait(NULL) > 0)
    {
    };

    for (int i = 0; i < printers_n; i++)
    {
        sem_destroy(&memory_map->printers[i].semaphore);
    }

    munmap(memory_map, sizeof(memory_map_t));

    shm_unlink(SHARED_MEMORY_NAME);
}