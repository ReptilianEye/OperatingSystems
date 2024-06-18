#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    pid_t pid;
    switch ((pid = fork())) {
        case 0:
            sleep(3);
            printf("Potomek: %d kończy działanie\n", getpid());
            exit(EXIT_SUCCESS);
        case -1:
            perror("FORK");
            exit(EXIT_FAILURE);
        default: {
            if (argc > 1)
                sleep(atoi(argv[1]));
            int err = kill(pid, SIGINT);
            if (err == -1) {
                exit(err);
            }
            /* Wyślij do potomka sygnał SIGINT i obsłuż błąd */
            int status;
            wait(&status);
            if (WIFEXITED(status)) {
                printf("Zakocznczyl przez exit %d, pid=%d\n", status, pid);
            } else if (WIFSIGNALED(status)) {
                printf("Zakonczyl przez sygnal %d\n", status);
                /* Czekaj na zakończenie potomka i pobierz status.
                   1) Jeśli potomek zakończył się normalnie (przez exit), wypisz informację wraz wartością statusu i jego PID.
                   2) Jeśli potomek zakończył się sygnałem, zwróć taką informację (wartość sygnału) */
            }
            return 1;
        }

            /* koniec */
            exit(0);
    }
}
