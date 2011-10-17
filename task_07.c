#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <mpi.h>

// This is manager program
//  it feeds input in workers
//  and gather their results

#define WORKER_PROGRAM "task_07_worker"
#define MIN(a, b) ((a) < (b) ? (a) : (b))

void read_matrix(FILE *file, double *M, int m, int n)
{
    for(int i = 0; i < m; ++i)
        for(int j = 0; j < n; ++j)
            fscanf(file, "%lf", &M[i * n + j]);
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

    double *bufferA = malloc(sizeof(double) * m * n);

    MPI_Finalize();

    free(A);
    free(B);

    return 0;
}
