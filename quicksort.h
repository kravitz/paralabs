#ifndef _QUICKSORT_H_
#define _QUICKSORT_H_

#include <stdlib.h>
#include "generic_sequence.h"

void quicksort(void *base, size_t num, size_t size, comparator_t comparator);

#endif
