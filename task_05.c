#include <stdio.h>
#include <stdlib.h>
#include "quicksort.h"
#include "ctest.h"

int is_ordered(void *base, size_t num, size_t size, comparator_t comparator)
{
    for(int i = 1; i < num; ++i)
    {
        if (comparator(at(base, size, i-1), at(base, size, i)) > 0)
            return 0;
    }
    return 1;
}

int int_comparator(const void* left, const void *right)
{
    return *(const int *)left - *(const int *)right;
}

#define LENGTH_AND_SIZE(A) LENGTH(A), sizeof(A[0])

int main()
{
    USE_CTEST

    BEGIN_TEST("Testing is_ordered validity with int array")
    {
        int array[] = {1,2,3,4,10}, array2[] = {2,4,2,3,5,10};
        CHECK(is_ordered(array, LENGTH_AND_SIZE(array), int_comparator));
        CHECK_NOT(is_ordered(array2, LENGTH_AND_SIZE(array2), int_comparator));
    }
    END_TEST;

    BEGIN_TEST("Basic quicksort")
    {
        int array[] = {3,2,1};
        quicksort(array, LENGTH_AND_SIZE(array), int_comparator);
        CHECK(is_ordered(array, LENGTH_AND_SIZE(array), int_comparator));
    }
    END_TEST;

    BEGIN_TEST("QuickSort on already sorted sequence")
    {
        int array[] = {1,2,3,4,5,6,7,8,9};
        CHECK(is_ordered(array, LENGTH_AND_SIZE(array), int_comparator));
        quicksort(array, LENGTH_AND_SIZE(array), int_comparator);
        CHECK(is_ordered(array, LENGTH_AND_SIZE(array), int_comparator));
    }
    END_TEST;

    BEGIN_TEST("Sorting long random sequence")
    {
        size_t length = 1e6;
        int *array = (int *)malloc(length * sizeof(int));
        CHECK(array != NULL);
        for(size_t i = 0; i < length; ++i)
        {
            array[i] = rand();
        }
        quicksort(array, length, sizeof(int), int_comparator);
        CHECK(is_ordered(array, length, sizeof(int), int_comparator));
        free(array);
    }
    END_TEST;

    SUMMARIZE

    return 0;
}
