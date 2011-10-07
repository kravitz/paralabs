#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>
#include "ctest.h"
#include "mergesort.h"
#include "generic_sequence.h"
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

    BEGIN_TEST("Basic mergesort")
    {
        int array[] = {3,2,1};
        mergesort(array, LENGTH_AND_SIZE(array), int_comparator);
        CHECK(is_ordered(array, LENGTH_AND_SIZE(array), int_comparator));
    }
    END_TEST;

    BEGIN_TEST("QuickSort on already sorted sequence")
    {
        int array[] = {1,2,3,4,5,6,7,8,9};
        CHECK(is_ordered(array, LENGTH_AND_SIZE(array), int_comparator));
        mergesort(array, LENGTH_AND_SIZE(array), int_comparator);
        CHECK(is_ordered(array, LENGTH_AND_SIZE(array), int_comparator));
    }
    END_TEST;

    struct timeval timev1,timev2;
    float time_seconds;

    BEGIN_TEST("Sorting reversed sequence")
    {
        size_t length = 1e6;
        int *array = (int *)malloc(length * sizeof(int));
        CHECK(array != NULL);
        for(size_t i = 0; i < length; ++i)
        {
            array[i] = length - i;
        }
        gettimeofday(&timev1,NULL);
        mergesort(array, length, sizeof(int), int_comparator);
        gettimeofday(&timev2,NULL);
        CHECK(is_ordered(array, length, sizeof(int), int_comparator));
        free(array);
    }
    END_TEST;
    SUMMARIZE;
    return 0;
}
