#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <error.h>
#include <queue>
#include <algorithm>
#include <limits.h>
#include <utility>
#include "maze_messages.h"
#include "maze.h"
using namespace std;

typedef int Coordinate;
typedef int Distance;
typedef pair<Coordinate, Distance> Point;

queue<Point> points_queue;
int rank;
int N, K;
int w, h;
int x, y;
char *maze;
int *distances;
int *borderVectors[4];
int borders[4];

enum Directions {Left = 0, Right, Up, Down};
int dx[] =      {  -1,    +1,  0,    0};
int dy[] =      {   0,     0, -1,   +1};

#define FREEZE_PROCCESS { int DebugWait = 1; \
    while (DebugWait) ; }

inline void add_to_queue(int x, int y, int distance)
{
    if (x >= 0 && y >= 0 && x < w && y < h)
    {
        int new_coord = y * w + x;
        if (maze[new_coord] != '#')
            points_queue.push(make_pair(new_coord, distance));
    }
}

void share_with(int to_x, int to_y, int i)
{
    if (to_x >= 0 && to_y >= 0 && to_x < K && to_y < K)
    {
        int row = 0, col = 0;
        int inc_row = 0, inc_col = 0;
        if (to_x == x)
            inc_col = 1;
        else if (to_x > x) col = w - 1;
        if (to_y == y)
            inc_row = 1;
        else if (to_y > y) row = h - 1;
        int *border = borderVectors[i];
        for (int j = 0; j < borders[i]; ++j)
        {
            border[j] = distances[row * w + col];
            row += inc_row;
            col += inc_col;
        }
        // how'll get this pile of shit?
        int rank_to = to_y * K + to_x;
        MPI_Send(border, borders[i], MPI_INTEGER, rank_to, PIECES_EXCHANGE, MPI_COMM_WORLD);
    }
}

int main(int argc, char *argv[])
{
    MPI_Comm parent;
    MPI_Init(&argc, &argv);
    MPI_Comm_get_parent(&parent);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (parent == MPI_COMM_NULL) error(1, 0, "No parent");
    {
        int NK[2];
        MPI_Bcast(&NK, 2, MPI_INT, 0, parent);
        N = NK[0];
        K = NK[1];
    }
    printf("rank %d: %d %d\n", rank, N, K);
    x = x_of_block(rank, K), y = y_of_block(rank, K);
    h = piece_size(N, K, y), w = piece_size(N, K, x);
    maze = new char[h * w];
    distances = new int [h * w];
    borders[(int) Left] = borders[(int) Right] = h;
    borders[(int) Up] = borders[(int) Down] = w;
    for (int i = 0; i < 4; ++i)
    {
        borderVectors[i] = new int[borders[i]];
    }

    fill(distances, distances + h * w, INT_MAX);
    MPI_Recv(maze, h * w, MPI_CHAR, 0, PIECE_OF_MAZE_TAG, parent, MPI_STATUS_IGNORE);

    int entry = contains_entry(maze, w, h);
    if (entry != -1)
    {
        Point point;
        points_queue.push(make_pair(entry, 0));
        while (!points_queue.empty())
        {
            point = points_queue.front();
            points_queue.pop();
            if (distances[point.first] > point.second)
            {
                distances[point.first] = point.second;
                if (maze[point.first] == 'E')
                {
                    // We found it!
                    MPI_Send(&point.second, 1, MPI_INTEGER, 0, GATHERING_RESULT, parent);
                }
                int x1 = point.first % w, y1 = point.first / w;
                int new_distance = point.second + 1;

                for (int d = (int) Left; d <= (int) Down; ++d)
                {
                    add_to_queue(x1 + dx[d], y1 + dy[d], new_distance);
                }
            }
        }
        /*
        for (int i = 0; i < h; ++i)
        {
            for (int j = 0; j < w; ++j)
            {
                printf("%d ", distances[i * w + j]);
            }
            printf("\n");
        }
        */
        for (int d = (int) Left; d <= (int) Down; ++d)
        {
            share_with(x + dx[d], y + dy[d], d);
            /*
            printf("\n");
            for (int i = 0; i < borders[d]; ++i)
            {
                printf("%d ", borderVectors[d][i]);
            }
            printf("\n");
            */
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        delete[] borderVectors[i];
    }
    delete[] maze;
    delete[] distances;
    MPI_Finalize();
    return 0;
}
