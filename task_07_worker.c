#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <error.h>
#include "matrix.h"
#include "messages.h"

#define FREEZE_PROCCESS { int DebugWait = 1; \
    while (DebugWait) ; }
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main(int argc, char *argv[])
{
    unsigned int K = atoi(argv[1]), M = atoi(argv[2]);
    MPI_Comm parent;
    MPI_Init(&argc, &argv);
    MPI_Comm_get_parent(&parent);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (parent == MPI_COMM_NULL) error(1, 0, "No parent");
    int m, n, q;
    {
        int mnq[3];
        MPI_Bcast(&mnq, 3, MPI_INT, 0, parent);
        m = mnq[0];
        n = mnq[1];
        q = mnq[2];
    }
    M = MIN(M, MIN(n, q));
    K = MIN(K, MIN(m, n));

    int xa = x_of_block(rank, M),
        ya = y_of_block(rank, M),
        ha = piece_size(m, K, ya),
        wa = piece_size(n, M, xa);
    double *bufferA = malloc(ha * wa * sizeof(double));
    MPI_Recv(bufferA, ha * wa, MPI_DOUBLE, 0, PIECE_OF_MATRIX_A_TAG, parent, MPI_STATUS_IGNORE);

    int yb = xa,
        xb = ya,
        hb = wa,
        wb = piece_size(q, K, xb);
    double *bufferB = malloc(hb * wb * sizeof(double));
    MPI_Recv(bufferB, hb * wb, MPI_DOUBLE, 0, PIECE_OF_MATRIX_B_TAG, parent, MPI_STATUS_IGNORE);
    MPI_Request pieces_exchange_reqs[K - 1];
    for (int y = 0, i = 0; y < K; ++y)
    {
        if (y == ya) continue; //self
        MPI_Isend(bufferB, hb * wb, MPI_DOUBLE, xa + y * M, PIECES_EXCHANGE, MPI_COMM_WORLD, pieces_exchange_reqs + i++);
    }

    double *pieces[K], *im_results[K];
    int piecesW[K];
    pieces[xb] = bufferB;
    for (int i = 0; i < K; ++i)
    {
        piecesW[i] = piece_size(q, K, i);
        im_results[i] = malloc(sizeof(double) * ha * piecesW[i]);
        for (int j = 0; j < ha * piecesW[i]; ++j) im_results[i][j] = 0;
        if (i == xb) continue;
        pieces[i] = malloc(sizeof(double) * hb * piecesW[i]);
        MPI_Recv(pieces[i], hb * piecesW[i], MPI_DOUBLE, yb + i * M, PIECES_EXCHANGE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    
    for (int t = 0; t < K; ++t)
    {
        int wc = piecesW[t];
        double *pB = pieces[t];
        for (int i = 0; i < ha; ++i)
            for (int j = 0; j < wc; ++j)
                for (int k = 0; k < wa; ++k)
                    im_results[t][i * wc + j] += bufferA[k + i * wa] * pB[k * wc + j];
    }
    free(bufferA); // no longer used

    if (xa == 0) // Im master (collector)
    {
        double *res_line = malloc(sizeof(double) * q * ha), *to_sum;
        for (int i = 0; i < q * ha; ++i) res_line[i] = 0;
        for (int i = 0, offset = 0; i < K; ++i)
        {
            int wc = piecesW[i], hc = ha;
            for (int j = 0; j < M; ++j)
            {
                if (j == 0)
                {
                    to_sum = im_results[i];
                }
                else
                {
                    to_sum = malloc(sizeof(double) * hc * wc); 
                    MPI_Recv(to_sum, hc * wc, MPI_DOUBLE, ya * M + j, GATHERING_RESULT + i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
                for (int x = 0; x < wc; ++x)
                    for (int y = 0; y < hc; ++y)
                        res_line[(x + offset) + y * q] += to_sum[x + y * wc];
                free(to_sum);
            }
            offset += wc;
        }
        MPI_Send(res_line, q * ha, MPI_DOUBLE, 0, GATHERING_RESULT + ya, parent);
        free(res_line);
    }
    else // Im slave
    {
        for(int i = 0; i < K; ++i)
        {
            MPI_Send(im_results[i], ha * piecesW[i], MPI_DOUBLE, 0 + ya * M, GATHERING_RESULT + i, MPI_COMM_WORLD);
        }
    }

    // just before free the buffer we need to know that everyone received our messages :)
    MPI_Waitall(K - 1, pieces_exchange_reqs, MPI_STATUS_IGNORE);
    for (int i = 0; i < K; ++i)
    {
        free(pieces[i]);
        if (xa != 0) free(im_results[i]);
    }

    MPI_Finalize();
}
