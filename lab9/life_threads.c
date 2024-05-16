#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid_threads.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <signal.h>

int main(int argc, char *argv[])
{
    int const grid_height = 30;
    int const grid_width = 30;
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <threads no.>\n", argv[0]);
        return 1;
    }

    int threads_no = atoi(argv[1]);
    int const cells_per_thread = (grid_height * grid_width) / threads_no + 0.5;

    srand(time(NULL));
    setlocale(LC_CTYPE, "");
    initscr(); // Start curses mode

    char *foreground = create_grid(grid_width, grid_height);
    char *background = create_grid(grid_width, grid_height);
    char *tmp;

    init_grid(foreground);

    pthread_t threads[threads_no];

    arg_struct_t args[threads_no];

    for (int i = 0; i < threads_no; i++)
    {
        int start = i * cells_per_thread;
        int end = (i + 1) * cells_per_thread;
        if (end > grid_height * grid_width)
        {
            end = grid_height * grid_width;
        }
        args[i].foreground = &foreground;
        args[i].background = &background;
        args[i].start = start;
        args[i].end = end;
        pthread_create(&threads[i], NULL, &update_grid, &args[i]);
    }

    while (true)
    {
        draw_grid(foreground);
        usleep(500 * 1000);

        // Step simulation
        for (int i = 0; i < threads_no; i++)
        {
            pthread_kill(threads[i], SIGUSR1);
        }
        tmp = foreground;
        foreground = background;
        background = tmp;
    }

    endwin(); // End curses mode
    destroy_grid(foreground);
    destroy_grid(background);

    return 0;
}
