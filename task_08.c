#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <string.h>
#include <ctype.h>
#include "maze.h"
#include "maze_messages.h"
#include <mpi.h>

#define WORKER_PROGRAM "task_08_worker"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s filename", argv[0]);
        return 1;
    }
    char *filename = argv[1];
    FILE *finput = fopen(filename, "r");
    int N, K;
    fscanf(finput, "%d %d", &N, &K);
    char *maze = malloc(N*N);
    for (int i = 0; i < N * N; ++i)
    {
        do
        {
            fscanf(finput, "%c", maze + i);
        } while (isspace(maze[i]));
    }
    fclose(finput);
    if (!verify_maze(maze, N, N))
    {
        printf("Maze is incorrect.\n");
        return 1;
    }


    {
        int rc = MPI_Init(&argc, &argv);
        if (rc != MPI_SUCCESS)
        {
            printf("Cann't start MPI program, exiting\n");
            MPI_Abort(MPI_COMM_WORLD, rc);
        }
    }
    MPI_Comm everyone;
    MPI_Comm_spawn(WORKER_PROGRAM, argv + 1, K * K, MPI_INFO_NULL, 0, MPI_COMM_SELF, &everyone, MPI_ERRCODES_IGNORE);

    {
        int NK[] = {N, K};
        MPI_Bcast(&NK, 2, MPI_INT, MPI_ROOT, everyone);
    }

    char *maze_to_send = malloc(N*N);
    char *target = maze_to_send;
    MPI_Request requests[K * K];
    for (int i = 0; i < K; ++i)
    {
        for (int j = 0; j < K; ++j)
        {
            int h = piece_size(N, K, i),
                w = piece_size(N, K, j),
                h_offset = piece_offset(N, K, i),
                w_offset = piece_offset(N, K, j),
                size = h * w,
                in_offset = w_offset + h_offset * N;
            for(int k = 0; k < h; ++k)
            {
                memcpy(target + k * w, maze + in_offset + k * N, w);
            }
            int rank = i * K + j;
            MPI_Isend(target, size, MPI_CHAR, rank, PIECE_OF_MAZE_TAG, everyone, requests + rank);
            target += size;
        }
    }
    MPI_Waitall(K * K, requests, MPI_STATUS_IGNORE);
    int length = 0;
    free(maze_to_send);
    free(maze);

    MPI_Finalize();

    return 0;
}
