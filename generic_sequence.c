#include "generic_sequence.h"

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

int is_ordered(void *base, size_t num, size_t size, comparator_t comparator)
{
    for(int i = 1; i < num; ++i)
    {
        if (comparator(at(base, size, i-1), at(base, size, i)) > 0)
            return 0;
    }
    return 1;
}

arithmetic_comparator(int)
arithmetic_comparator(char)
