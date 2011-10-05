#ifndef _QUICKSORT_H_
#define _QUICKSORT_H_

#include <stdlib.h>

typedef int (* comparator_t) (const void *, const void *);
void quicksort(void *base, size_t num, size_t size, comparator_t comparator);
void *at(void *base, size_t size, int shift);

#endif
