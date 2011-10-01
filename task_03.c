#include <stdio.h>
#include "tasks.h"

#define N 120
int counter = 0;
int printed = 1;

pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t printed_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t counter_even = PTHREAD_COND_INITIALIZER;
pthread_cond_t counter_odd = PTHREAD_COND_INITIALIZER;
pthread_cond_t counter_changed = PTHREAD_COND_INITIALIZER;
pthread_cond_t counter_printed = PTHREAD_COND_INITIALIZER;

void modifiers_helper(int val, pthread_cond_t *signal)
{
    counter = val;
    pthread_mutex_lock(&printed_mutex);
    printed = 0;

    pthread_cond_signal(&counter_changed);
    if (!printed) pthread_cond_wait(&counter_printed, &printed_mutex);
    pthread_mutex_unlock(&printed_mutex);
    pthread_cond_signal(signal);
    pthread_mutex_unlock(&counter_mutex);
}

void *even_modifier(void *arg)
{
    int i;
    for (i = 2; i <= N; i += 2)
    {
        pthread_mutex_lock(&counter_mutex);
        
        if (!(counter % 2)) pthread_cond_wait(&counter_odd, &counter_mutex);

        modifiers_helper(i, &counter_even);
    }
}

void *odd_modifier(void *arg)
{
    int i;
    for (i = 1; i <= N; i += 2)
    {
        pthread_mutex_lock(&counter_mutex);
        
        if (counter % 2) pthread_cond_wait(&counter_even, &counter_mutex);
        
        modifiers_helper(i, &counter_odd);
    }
}

void *printer(void *arg)
{
    int lcounter;
    do 
    {
        pthread_mutex_lock(&printed_mutex);
        if (printed) pthread_cond_wait(&counter_changed, &printed_mutex);
        printf("%d\n", lcounter = counter);
        printed = 1;
        pthread_cond_signal(&counter_printed);
        pthread_mutex_unlock(&printed_mutex);
    } while (lcounter < N);
}

int main()
{
    pthread_t even_modifier_thread, odd_modifier_thread, printer_thread;

    pthread_create(&printer_thread, NULL, printer, NULL);
    pthread_create(&even_modifier_thread, NULL, even_modifier, NULL);
    pthread_create(&odd_modifier_thread, NULL, odd_modifier, NULL);    
    pthread_join(printer_thread, NULL);

    return 0;
}
