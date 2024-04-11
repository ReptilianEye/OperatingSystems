#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

double f(double x)
{
    return 4 / (x * x + 1);
}
double calculate_integral(double l, double r, double h, double i)
{
    double x = l + i * h + h / 2;
    return h * f(x);
}
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s <n>\n", argv[0]);
        return 1;
    }
    const double l = 0;
    const double r = 1;

    double h;
    int n;
    sscanf(argv[1], "%lf", &h);
    sscanf(argv[2], "%d", &n);
    int fd[n][2];
    for (int i = 0; i < n; i++)
    {
        if (pipe(fd[i]) < 0)
        {
            perror("pipe");
            exit(1);
        }
        pid_t pid = fork();
        if (pid == 0)
        {
            close(fd[i][0]);
            double res = calculate_integral(l, r, h, i);

            write(fd[i][1], &res, sizeof(res));
            exit(0);
        }
        close(fd[i][1]);
    }
    double result = 0;
    for (int i = 0; i < n; i++)
    {
        double res;
        read(fd[i][0], &res, sizeof(res));
        result += res;
    }
    printf("Result: %lf\n", result);
    return 0;
}