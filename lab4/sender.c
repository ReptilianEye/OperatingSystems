#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int globalVar = 0;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Too few arguments\n");
        return -1;
    }

    printf("filename: %s\n", argv[0]);

    int localVar = 0;

    pid_t pid = fork();

    if (pid == 0)
    {
        printf("child process");
        globalVar++;
        localVar++;
        printf("Child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", localVar, globalVar);

        int status = execl("/bin/ls", argv[1], "-l", NULL);

        exit(status);
    }
    int status;
    wait(&status);
    int childStatus = WEXITSTATUS(status);
    printf("parent process");
    printf("Parent pid = %d, child pid = %d\n", getpid(), pid);
    printf("Child exit code: %d\n", childStatus);
    printf("parent's local = %d, parent's global = %d\n", localVar, globalVar);

    return childStatus;
}