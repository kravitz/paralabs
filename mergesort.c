#include <string.h>
#include "mergesort.h"
#include "quicksort.h"
#include "generic_sequence.h"

int K = 29;

void setK(int newK)
{
    K = newK;
}

// ... | base1 ... l1 elements | base2 ... l2_elements | ...
void merge(void *base1, void *base2, size_t l1, size_t l2, size_t el_size, comparator_t comparator)
{
    void *L = (void *)malloc(l1 * el_size),
         *R = (void *)malloc(l2 * el_size);
    memcpy(L, base1, el_size * l1);
    memcpy(R, base2, el_size * l2);

    int i = 0, j = 0;
    for (int k = 0; k < l1 + l2; ++k)
    {
        if(j == l2 || (i != l1 && comparator(at(L, el_size, i), at(R, el_size, j)) <= 0))
        {
            memcpy(at(base1, el_size, k), at(L, el_size, i++), el_size);
        }
        else
        {
            memcpy(at(base1, el_size, k), at(R, el_size, j++), el_size);
        }
    }

    free(L);
    free(R);
}

/* funny variant of mergesort, turned upside-down */
// coffee and insomnia (tm)
void mergesort(void *base, size_t length, size_t el_size, comparator_t comparator)
{
    int div = K;
    void *b1, *b2;
    #pragma omp parallel for
    for (int i = 0; i < length / div; ++i)
    {
        quicksort(at(base, el_size, i * div), div, el_size, comparator);
    }
    if (length % div)
    {
        quicksort(at(base, el_size, (length / div) * div), length % div, el_size, comparator);
    }
    // now repeat merge phase
    while(1)
    {
        size_t intervals_count = (length / div);
        size_t pairs_count = intervals_count / 2;
        if (intervals_count < 2 && !(intervals_count == 1 && (length % div))) break;
        #pragma omp parallel for private(b1, b2)
        for (int j = 0; j < pairs_count; ++j)
        {
            b1 = at(base, el_size, (2*j) * div);
            b2 = at(base, el_size, (2*j + 1) * div); 
            merge(b1, b2, div, div, el_size, comparator);
        }
        if (intervals_count % 2 && length % div)
        {
            b1 = at(base, el_size, (intervals_count - 1) * div);
            b2 = at(base, el_size, intervals_count * div);
            merge(b1, b2, div, length % div, el_size, comparator);
        }
        div *= 2;
    }
}
