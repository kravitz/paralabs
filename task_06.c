#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
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
    time_seconds = timev2.tv_sec-timev1.tv_sec+0.000001*(timev2.tv_usec-timev1.tv_usec);
    printf("Time of sorting in this test: %.4f \n", time_seconds);

    size_t length = 5e6;
    size_t array_byte_size = length * sizeof(int);
    int *original_array = (int *)malloc(array_byte_size),
        *sorted_array = (int *)malloc(array_byte_size);
    for(int i = 0; i < length; ++i) original_array[i] = rand(); 
    char title[256];
    for(int num_threads = 1; num_threads < 8; num_threads++)
    {
        sprintf(title, "Sorting long random sequence with %d threads", num_threads);
        omp_set_num_threads(num_threads);
        BEGIN_TEST(title)
        {
            memcpy(sorted_array, original_array, array_byte_size);
            gettimeofday(&timev1,NULL);
            mergesort(sorted_array, length, sizeof(int), int_comparator);
            gettimeofday(&timev2,NULL);
            CHECK(is_ordered(sorted_array, length, sizeof(int), int_comparator));
        }
        END_TEST;
        time_seconds = timev2.tv_sec-timev1.tv_sec+0.000001*(timev2.tv_usec-timev1.tv_usec);
        printf("Time of sorting in this test: %.4f \n", time_seconds);
    }
    /*
    printf("Determine good thread count and K");
    struct mark {
        float time;
        int K;
    };
    struct mark mink[10];

    for(int num_threads = 1; num_threads < 10; num_threads++)
    {
        float mintime = 1e9; // should be enough :)
        for(int k = 2; k < 50; ++k)
        {
            memcpy(sorted_array, original_array, array_byte_size);
            setK(k);
            gettimeofday(&timev1,NULL);
            mergesort(sorted_array, length, sizeof(int), int_comparator);
            gettimeofday(&timev2,NULL);
            time_seconds = timev2.tv_sec-timev1.tv_sec+0.000001*(timev2.tv_usec-timev1.tv_usec);
            if (time_seconds < mintime)
            {
                mintime = time_seconds;
                mink[num_threads].time = mintime;
                mink[num_threads].K = k;
            }
            printf("k = %d, %d threads: %.4f \n", k, num_threads, time_seconds);
        }
    }
    printf("Measure finished, results:\n");
    for(int i = 1; i < 10; ++i)
    {
        printf("Threads %d, K: %d, time: %.4f\n", i, mink[i].K, mink[i].time);
    }
    */
    free(sorted_array);
    free(original_array);

    SUMMARIZE;

    return 0;
}
