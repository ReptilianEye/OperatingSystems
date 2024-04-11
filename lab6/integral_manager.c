#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Usage: %s <a> <b> <n>\n", argv[0]);
        return 1;
    }
    double a, b;
    int n;
    a = atof(argv[1]);
    b = atof(argv[2]);
    n = atoi(argv[3]);

    mkfifo("input", 0666);
    mkfifo("output", 0666);

    int fd = open("input", O_WRONLY);
    if (fd == -1)
    {
        printf("Cannot open input file\n");
        return -1;
    }
    write(fd, &a, sizeof(double));
    write(fd, &b, sizeof(double));
    write(fd, &n, sizeof(int));
    close(fd);
    printf("Waiting for result...\n");

    fd = open("output", O_RDONLY);
    if (fd == -1)
        return -1;
    double result;
    read(fd, &result, sizeof(double));
    close(fd);

    printf("Result: %lf\n", result);
    return 0;
}