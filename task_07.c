#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>
#include <mpi.h>
#include "matrix.h"
#include "messages.h"

// This is manager program
//  it feeds input in workers
//  and gather their results

#define FREEZE_PROCCESS { int DebugWait = 0; \
    while (DebugWait) ; }
#define WORKER_PROGRAM "task_07_worker"
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void read_matrix(FILE *file, double *M, int m, int n)
{
    for(int i = 0; i < m; ++i)
        for(int j = 0; j < n; ++j)
            fscanf(file, "%lf", &M[i * n + j]);
}

// Works on both dimensions

int main(int argc, char *argv[])
{
    MPI_Comm everyone;
    if (argc != 3)
    {
        printf("Usage: %s M K\n where M - split columns count, K - split rows count\n", argv[0]);
        return 1;
    }
    unsigned int K = atoi(argv[1]), M = atoi(argv[2]);
    if (K <= 0 || K > 3 || M <= 0 || M > 3) error(1, 0, "K or M exceeds [1,3] range\n");

    FILE *finput = fopen("task_07_input.txt", "r");
    int m, n, q;
    fscanf(finput, "%d\n%d\n%d\n", &m, &n, &q);
    if (m <= 1 || n <= 1 || q <= 1) error(1, 0, "m, n and q should be > 1\n");
    // reassign split values, if matrices are too small
    M = MIN(M, MIN(n, q));
    K = MIN(K, MIN(m, n));
    size_t bsize_A = sizeof(double) * m * n, bsize_B = sizeof(double) * m * n;
    double *A = malloc(bsize_A);
    double *B = malloc(bsize_B);
    read_matrix(finput, A, m, n);
    read_matrix(finput, B, n, q);
    fclose(finput);

    int rc = MPI_Init(&argc, &argv);
    if (rc != MPI_SUCCESS)
    {
        printf("Cann't start MPI program, exiting\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }
    MPI_Comm_spawn(WORKER_PROGRAM, argv + 1, K * M, MPI_INFO_NULL, 0, MPI_COMM_SELF, &everyone, MPI_ERRCODES_IGNORE);

    {
        int mnq[] = {m, n, q};
        MPI_Bcast(&mnq, 3, MPI_INT, MPI_ROOT, everyone);
    }

    double *bufferA = malloc(bsize_A), *bufferB = malloc(bsize_B);
    double *offsetA = bufferA, *offsetB = bufferB; 
    MPI_Request requests[K * M * 2];
    for (int i = 0; i < K; ++i)
    {
        for (int j = 0; j < M; ++j)
        {
            int h = piece_size(m, K, i),
                w = piece_size(n, M, j),
                h_offset = piece_offset(m, K, i),
                w_offset = piece_offset(n, M, j),
                size = h * w,
                in_offset = w_offset + h_offset * n;
            for(int k = 0; k < h; ++k)
            {
                memcpy(offsetA + k * w, A + in_offset + k * n, w * sizeof(double));
            }
            int rank = i * M + j;
            MPI_Isend(offsetA, size, MPI_DOUBLE, rank, PIECE_OF_MATRIX_A_TAG, everyone, requests + rank);
            offsetA += size;
        }
    }
    free(A);
    MPI_Request *requestsB = requests + K * M;
    for (int i = 0; i < K; ++i)
    {
        for (int j = 0; j < M; ++j)
        {
            int h = piece_size(n, M, j),
                w = piece_size(q, K, i),
                h_offset = piece_offset(n, M, j),
                w_offset = piece_offset(q, K, i),
                size = h * w,
                in_offset = w_offset + h_offset * q;
            for(int k = 0; k < h; ++k)
            {
                memcpy(offsetB + k * w, B + in_offset + k * q, w * sizeof(double));
            }
            int rank = i * M + j;
            MPI_Isend(offsetB, size, MPI_DOUBLE, rank, PIECE_OF_MATRIX_B_TAG, everyone, requestsB + rank);
            offsetB += size;
        }
    }
    MPI_Waitall(K * M * 2, requests, MPI_STATUS_IGNORE);
    free(bufferA);
    free(bufferB);
    free(B);

    double *result = malloc(sizeof(double) * m * q);
    for (int i = 0; i < K; ++i)
    {
        MPI_Status st;
        int count;
        MPI_Recv(result + q * piece_offset(m, K, i), q * piece_size(m, K, i), MPI_DOUBLE, i * M, GATHERING_RESULT + i, everyone, &st);
        MPI_Get_count(&st, MPI_DOUBLE, &count);
        printf("Received %d values of result\n", count);
    }
    print_matrix(result, m, q);
    free(result);

    MPI_Finalize();

    return 0;
}
