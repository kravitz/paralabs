#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void check_pthread(int retcode, char* file, int line)
{
    if (retcode != 0)
    {
        fprintf(stderr, "pthread error on %s:%d", file, line);
        exit(retcode);
    }
}

#define PCheck(call) check_pthread(call, __FILE__, __LINE__)
