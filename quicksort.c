#include "quicksort.h"
#include "generic_sequence.h"
#include <stdio.h>

size_t partition(void *base, size_t num, size_t size, comparator_t comparator)
{
    void *px = at(base, size, num - 1), *pj;
    int i = -1;
    for(int j = 0; j < num - 1; j++)
    {
        pj = at(base, size, j);
        if(comparator(pj, px) <= 0)
        {
            swap(at(base, size, ++i), pj, size);
        }
    }
    swap(at(base, size, ++i), px, size);
    return i;
}

/*
void insertionsort(void *base, size_t num, size_t size, comparator_t comparator)
{
    int i, j;
#pragma parallel for private(j)
    for (i = 1; i < num; i++) {
        j = i;
        while (j > 0 && comparator(at(base, size, j - 1), at(base, size, j)) > 0) {
            swap(at(base, size, j), at(base, size, j - 1), size);
            j--;
        }
    }
}*/

void quicksort(void *base, size_t num, size_t size, comparator_t comparator)
{
    if (num > 1)
    {
        swap(at(base, size, num - 1), at(base, size, rand() % num), size); // Randomize
        int q = partition(base, num, size, comparator);
        #pragma omp parallel sections
        {
            #pragma omp section
            quicksort(base, q, size, comparator);
            #pragma omp section
            quicksort(at(base, size, q + 1), num - q - 1, size, comparator); 
        }
    }
    /*
    else if (num > 1)
    {
        insertionsort(base, num, size, comparator);
    }*/
}

