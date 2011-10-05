#include "quicksort.h"
#include <stdio.h>

void *at(void *base, size_t size, int shift)
{
    return (char *)base + shift * size;
}

void swap(void *a, void *b, size_t size)
{
    char c;
    for (int i = 0; i < size; ++i)
    {
        c = ((char *)a)[i];
        ((char *)a)[i] = ((char *)b)[i];
        ((char *)b)[i] = c;
    }
}

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

void quicksort(void *base, size_t num, size_t size, comparator_t comparator)
{
    if (num > 1)
    {
        swap(at(base, size, num - 1), at(base, size, rand() % num), size); // Randomize
        int q = partition(base, num, size, comparator);
        quicksort(base, q, size, comparator);
        quicksort(at(base, size, q + 1), num - q - 1, size, comparator); 
    }
}

