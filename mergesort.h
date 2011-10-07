#ifndef _MERGESORT_H_
#define _MERGESORT_H_

#include <stdlib.h>
#include "generic_sequence.h"

void mergesort(void *base, size_t length, size_t el_size, comparator_t comparator);

#endif
