#include <stdio.h>
#include "libcollatz.h"

int main()
{
    int result = test_collatz_convergence(27, 100);
    printf("Result: %d\n", result);
    return 0;
}
