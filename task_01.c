#include <stdlib.h>
#include <stdio.h>
#include "tasks.h"

#define ITERATIONS_COUNT 10000000
#define DIGITS_TO_PRINT 20

void *compute_e(void *arg)
{
    long double e_approx = 1, n = 1;
    int i;
    printf("starting computation\n");
    for (i = 1; i <= ITERATIONS_COUNT; ++i)
    {
        e_approx += n /= i;
    }
    printf("computation finished in " STR(ITERATIONS_COUNT) " iterations\n");
    printf("e = %." STR(DIGITS_TO_PRINT) "Lf\n", e_approx);
    pthread_exit(NULL);
}

void *wait(void *arg)
{
    pthread_t computer_thread = (pthread_t) arg;
    pthread_join(computer_thread, NULL);
    printf("after join\n");
}

int main(const int argc, const char *argv[])
{
    pthread_t computer_thread, waiter_thread;

    pthread_create(&computer_thread, NULL, compute_e, NULL);
    pthread_create(&waiter_thread, NULL, wait, (void *) computer_thread);
    printf("both threads spawned\n");
    pthread_join(waiter_thread, NULL);

    pthread_exit(NULL);
    return 0;
}
