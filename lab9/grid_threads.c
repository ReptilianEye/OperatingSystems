#include "grid_threads.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>

int grid_width = 30;
int grid_height = 30;

char *create_grid(int g_width, int g_height)
{
    grid_height = g_height;
    grid_width = g_width;
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}
void handler(int signum)
{
    return;
}
void *update_grid(void *args)
{
    arg_struct_t *arguments = (arg_struct_t *)args;
    int start = arguments->start;
    int end = arguments->end;
    signal(SIGUSR1, handler);

    char *tmp;
    while (1)
    {
        // printf("src: %p, dst: %p\n", src, dst);
        pause();
        for (int i = start; i < end; ++i)
        {
            (*arguments->background)[i] = is_alive(i / grid_width, i % grid_width, *arguments->foreground);
        }
    }
}