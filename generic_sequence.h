#ifndef _GENERAL_SEQUENCE_H_
#define _GENERAL_SEQUENCE_H_

#include <stdlib.h>

void *at(void *base, size_t size, int shift);
void swap(void *a, void *b, size_t size);
typedef int (* comparator_t) (const void *, const void *);
int is_ordered(void *base, size_t num, size_t size, comparator_t comparator);

#define arithmetic_comparator_definition(type) \
    type type ## _comparator(const void* left, const void *right);
#define arithmetic_comparator(type) \
    type type ## _comparator(const void* left, const void *right) \
    { \
        return *(const type *)left - *(const type *)right; \
    }

arithmetic_comparator_definition(int)
arithmetic_comparator_definition(char)

#endif
