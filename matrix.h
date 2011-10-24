#ifndef _MATRIX_H_
#define _MATRIX_H_
#include <stdlib.h>

int piece_size(size_t length, size_t count, size_t at);
int piece_offset(size_t length, size_t count, size_t at);

int x_of_block(int rank, int M);
int y_of_block(int rank, int M);

void print_matrix(double *M, int m, int n);

#endif
