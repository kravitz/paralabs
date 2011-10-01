#include <stdio.h>
#include "tasks.h"

#define N 120
int counter = 0;
pthread_mutex_t counter_mutex;
pthread_cond_t counter_even;
pthread_cond_t counter_odd;


void *even_modifier(void *arg)
{
    int i;
    for (i = 2; i <= N; i += 2)
    {
        pthread_mutex_lock(&counter_mutex);
        if (!(counter % 2))
        {
            pthread_cond_wait(&counter_odd, &counter_mutex);
        }
        
        counter = i;
        printf("%d\n", counter);
        pthread_cond_signal(&counter_even);
        pthread_mutex_unlock(&counter_mutex);
    }
}

void *odd_modifier(void *arg)
{
    int i;
    for (i = 1; i <= N; i += 2)
    {
        pthread_mutex_lock(&counter_mutex);
        if (counter % 2)
        {
            pthread_cond_wait(&counter_even, &counter_mutex);
        }
        
        counter = i;
        printf("%d\n", counter);
        pthread_cond_signal(&counter_odd);
        pthread_mutex_unlock(&counter_mutex);
    }
}

int main()
{
    pthread_t even_modifier_thread, odd_modifier_thread;
    
    pthread_mutex_init(&counter_mutex, NULL);
    pthread_cond_init(&counter_even, NULL);
    pthread_cond_init(&counter_odd, NULL);
    pthread_create(&even_modifier_thread, NULL, even_modifier, NULL);
    pthread_create(&odd_modifier_thread, NULL, odd_modifier, NULL);
    pthread_join(even_modifier_thread, NULL);
    pthread_join(odd_modifier_thread, NULL);
    pthread_mutex_destroy(&counter_mutex);
    pthread_cond_destroy(&counter_even);
    pthread_cond_destroy(&counter_odd);

    return 0;
}
