#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Too few arguments\n");
        return -1;
    }

    long toSpawn = strtol(argv[1], NULL, 10);

    if (toSpawn < 0)
    {
        printf("Number of processes must be greater than 0\n");
        return -1;
    }

    for (int i = 0; i < toSpawn; i++)
    {
        pid_t pid = fork();

        if (pid == 0)
        {
            printf("Child process pid %d\n", getpid());
            printf("Parent process pid %d\n", getppid());
            exit(0);
        }
    }

    while (wait(NULL) > 0)
    {
    };

    printf("Spawned processes: %s\n", argv[1]);

    return 0;
}