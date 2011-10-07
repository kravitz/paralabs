#include <string.h>
#include "mergesort.h"
#include "quicksort.h"
#include "generic_sequence.h"
#define K 17

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

void mergesort(void *base, size_t length, size_t el_size, comparator_t comparator)
{
    if (length > K)
    {
        size_t q = length >> 1, l2 = length - q;
        void *base2 = at(base, el_size, q);
        #pragma omp parallel sections
        {
            #pragma omp section
            mergesort(base, q, el_size, comparator);
            #pragma omp section
            mergesort(base2, l2, el_size, comparator);
        }
        merge(base, base2, q, l2, el_size, comparator);
    }
    else
        quicksort(base, length, el_size, comparator);
}
