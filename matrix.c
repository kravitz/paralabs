#include <stdio.h>
#include "matrix.h"

int piece_size(size_t length, size_t count, size_t at)
{
    return length / count + ((length % count && at == (count - 1)) ? length % count : 0);
}

int piece_offset(size_t length, size_t count, size_t at)
{
    return piece_size(length, count, 0) * at;
}

int x_of_block(int rank, int M)
{
    return rank % M;
}

int y_of_block(int rank, int M)
{
    return rank / M;
}

void print_matrix(double *M, int m, int n)
{
    for(int i = 0; i < m; ++i)
    {
        for(int j = 0; j < n; ++j)
            printf("%lf ", M[i * n + j]);
        printf("\n");
    }
}
