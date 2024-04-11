#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>

int current_status = -1;
int changes_cnt = 0;

void handle_argument(int argument)
{
    current_status = argument;
    changes_cnt++;
}

void handler_for_SIGUSR1(int signum, siginfo_t *info, void *context)
{
    int int_signal = info->si_int;
    printf("Signal %d received with value %d\n", signum, int_signal);

    handle_argument(int_signal);

    kill(info->si_pid, SIGUSR1);
}

int main()
{

    printf("Catcher PID: %d\n", getpid());

    struct sigaction action;
    action.sa_sigaction = handler_for_SIGUSR1;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);

    sigaction(SIGUSR1, &action, NULL);

    while (1)
    {
        switch (current_status)
        {
        case 1:
            for (int i = 1; i <= 100; i++)
                printf("%d ", i);
            printf("\n");
            current_status = -1;
            break;
        case 2:
            printf("Changes count: %d\n", changes_cnt);
            current_status = -1;
            break;
        case 3:
            printf("Exiting...\n");
            exit(0);
            break;
        default:
            break;
        }
    }
    return 0;
}