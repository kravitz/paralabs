#include <stdio.h>
#include <stdlib.h>

int main(const int argc, const char *argv[])
{
    unsigned int N, M;
    if (argc != 3)
    {
        printf("Usage: %s N M\n where N - split columns count, M - split rows count", argv[0]);
        return 1;
    }
    N = atoi(argv[1]);
    M = atoi(argv[2]);
    printf("%d %d", N, M);
    return 0;
}
