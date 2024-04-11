#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

double f(double x)
{
    return 4 / (x * x + 1);
}

int main()
{
    double a, b;
    int n;

    int fd = open("input", O_RDONLY);
    if (fd == -1)
        return -1;

    read(fd, &a, sizeof(double));
    read(fd, &b, sizeof(double));
    read(fd, &n, sizeof(int));
    close(fd);

    printf("Calculating integral from %lf to %lf with %d steps\n", a, b, n);

    double h = (b - a) / n;
    double result = 0;
    for (int i = 0; i < n; i++)
    {
        double x = a + i * h + h / 2;
        result += h * f(x);
    }
    fd = open("output", O_WRONLY);
    if (fd == -1)
        return -1;
    write(fd, &result, sizeof(double));
    close(fd);
    printf("Calculated result: %lf\n", result);
}