#include <stdio.h>
#include "collatz.h"

int main()
{
    int result = test_collatz_convergence(15, 100);
    printf("Result: %d\n", result);
    return 0;
}
