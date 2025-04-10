#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if(argc != 5)
    {
        return -1;
    }

    FILE *file;
    file = fopen(argv[1], "w");

    unsigned int vertices, maxWeight;
    unsigned long edges;
    sscanf(argv[2], "%u", &vertices);
    sscanf(argv[3], "%lu", &edges);
    sscanf(argv[4], "%u", &maxWeight);

    srand(1);
    for(unsigned long i = 0; i < edges; i++)
    {
        unsigned int a = rand() % vertices;
        unsigned int b = rand() % vertices;
        unsigned  w = (rand() % maxWeight) + 1;
        fprintf(file, "%u %u %u\n", a, b, w);
    }
}