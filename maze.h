#ifndef _MAZE_H
#define _MAZE_H

extern inline int piece_size(size_t length, size_t count, size_t at)
{
    return length / count + ((length % count && at == (count - 1)) ? length % count : 0);
}

extern inline int piece_offset(size_t length, size_t count, size_t at)
{
    return piece_size(length, count, 0) * at;
}

extern inline int x_of_block(int rank, int M)
{
    return rank % M;
}

extern inline int y_of_block(int rank, int M)
{
    return rank / M;
}

void print_maze(char *maze, int w, int h)
{
    for (int i = 0; i < h; ++i)
    {
        for (int j = 0; j < w; ++j)
        {
            printf("%c", maze[i*h + j]);
        }
        printf("\n");
    }
}

// entry should be only one
int contains_entry(char *maze, int w, int h)
{
    int has_entry_point = -1;
    for (int i = 0; i < w * h; ++i)
    {
        if (maze[i] == '*')
        {
            if(has_entry_point == -1)
                has_entry_point = i;
            else
                return -1;
        }
    }
    return has_entry_point;
}

int contains_exit(char *maze, int w, int h)
{
    int has_exit = 0;
    for (int i = 0; i < w * h; ++i)
    { // Its bitwise, but still ok
        has_exit |= (maze[i] == 'E');
    }
    return has_exit;
}

extern inline int verify_maze(char *maze, int w, int h)
{
    return (contains_entry(maze, w, h) != -1) && contains_exit(maze, w, h);
}

#endif
