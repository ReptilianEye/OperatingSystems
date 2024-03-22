#include <stdio.h>

int main(int argc, char const *argv[])
{
    printf("argc: %s\n", argv[1]);
    printf("argc: %s\n", argv[2]);
    FILE *in_file = fopen(argv[1], "r");
    FILE *out_file = fopen(argv[2], "a+");

    if (!in_file || !out_file)
    {
        printf("Error opening file\n");
        return 1;
    }
    fseek(in_file, -1, SEEK_END);

    char c;
    for (long bytesToRead = ftell(in_file) + 1; bytesToRead > 0; bytesToRead--, fseek(in_file, -2, SEEK_CUR))
    {
        c = fgetc(in_file);
        fprintf(out_file, "%c", c);
    }

    fclose(in_file);
    fclose(out_file);
    return 0;
}
