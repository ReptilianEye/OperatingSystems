#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>

void handler(int signum)
{
    printf("Signal %d received\n", signum);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <pid>\n", argv[0]);
        return 1;
    }
    if (!strcmp(argv[1], "none"))
    {
        signal(SIGUSR1, SIG_DFL);
        raise(SIGUSR1);
        return 0;
    }
    if (!strcmp(argv[1], "ignore"))
    {
        signal(SIGUSR1, SIG_IGN);
        raise(SIGUSR1);
        return 0;
    }
    if (!strcmp(argv[1], "handler"))
    {
        signal(SIGUSR1, handler);
        raise(SIGUSR1);
        return 0;
    }
    if (!strcmp(argv[1], "mask"))
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);

        sigprocmask(SIG_SETMASK, &mask, NULL);
        raise(SIGUSR1);

        sigset_t pending;
        sigpending(&pending);

        printf("Signal %d is %s\n", SIGUSR1, sigismember(&pending, SIGUSR1) ? "pending" : "not pending");

        return 0;
    }
    return 0;
}