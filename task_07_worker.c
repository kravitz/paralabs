#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <error.h>

int main(int argc, char *argv[])
{
    unsigned int N = atoi(argv[1]), M = atoi(argv[2]);
    MPI_Comm parent;
    MPI_Init(&argc, &argv);
    MPI_Comm_get_parent(&parent);
    if (parent == MPI_COMM_NULL) error(1, 0, "No parent");
    int m, n, q;
    {
        int mnq[3];
        MPI_Bcast(&mnq, 3, MPI_INT, 0, parent);
        m = mnq[0];
        n = mnq[1];
        q = mnq[2];
    }
    printf("%d %d %d\n", m, n, q);
    //printf("In worker: %s", msg);
    //int rank;
    //MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Finalize();
}
