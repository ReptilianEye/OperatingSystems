#pragma once
#include <stdbool.h>

typedef struct
{
    char **foreground;
    char **background;
    int start;
    int end;
} arg_struct_t;
char *create_grid(int g_width, int g_height);
void destroy_grid(char *grid);
void draw_grid(char *grid);
void init_grid(char *grid);
bool is_alive(int row, int col, char *grid);
void *update_grid(void *args);
