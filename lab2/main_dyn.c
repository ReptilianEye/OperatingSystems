#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main()
{
    char *error;
    void *handle = dlopen("libcollatz.so", RTLD_LAZY);
    if (!handle)
    {
        fputs(dlerror(), stderr);
        exit(1);
    }

    int (*convergence)(int, int);
    convergence = dlsym(handle, "test_collatz_convergence");

    if ((error = dlerror()) != NULL)
    {
        fputs(error, stderr);
        exit(1);
    }
    int res = (*convergence)(15, 100);
    printf("Result: %d\n", res);

    dlclose(handle);
}