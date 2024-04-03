#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>

void handler_for_SIGUSR1(int signum)
{
    printf("Confirmation signal %d received\n", signum);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        for (int i = 0; i < argc; i++)
        {
            printf("Argument %d: %s\n", i, argv[i]);
        }
        printf("Not enough arguments\n");
        return 1;
    }

    printf("Sender PID: %d\n", getpid());

    signal(SIGUSR1, handler_for_SIGUSR1);

    long catcher_pid = strtol(argv[1], NULL, 10);
    int argument = strtol(argv[2], NULL, 10);

    union sigval value = {argument};

    sigset_t mask;
    sigfillset(&mask);

    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGINT);

    sigqueue(catcher_pid, SIGUSR1, value);
    printf("Signal %d sent to catcher PID: %ld\n", SIGUSR1, catcher_pid);

    sigsuspend(&mask);
    return 0;
}