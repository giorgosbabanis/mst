#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define RESET "\x1B[0m"

void printFromList(unsigned long *offsetList, unsigned int* edgeList, unsigned int id)
{
    unsigned long start = offsetList[id];
    unsigned long end = offsetList[id + 1];
    printf("id: %u count:%d list:\n", id, (int)(end - start));
    for(int i = start; i < end; i++)
    {
        printf(GRN "%u " RESET, edgeList[2 * i]);
        printf(RED "%u " RESET, edgeList[2 * i + 1]);
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        return -1;
    }
    int fd = open(argv[1], O_RDONLY);
    struct stat st;
    size_t size = fstat(fd, &st);
    unsigned long* graph = (unsigned long *)mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    unsigned long vertices = graph[0], edges = graph[1];
    unsigned long *offsetList = &graph[2];
    unsigned int *edgesList = (unsigned int*)&offsetList[vertices + 2];
    while (1)
        {
            unsigned int id, scanned;
            char op;
            printf("Give an id to print from new graph\n");
            scanned = scanf("%c %u", &op, &id);
            if (op == 'p')
            {
                printFromList(offsetList, edgesList, id);
            }
            
            if (op == 'x')
            {
                break;
            }
        }
}