#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define RESET "\x1B[0m"

const int vertexSize = 16;
const unsigned int vertexBufferSize = 1024;

typedef struct vertex
{
    unsigned int id;
    int count;
    unsigned int *neigbhors;
    struct extention *next;
    struct extention *last;
} vertex;

typedef struct extention
{
    int id;
    int count;
    unsigned int *neigbhors;
    struct extention *next;
} extention;

void allocateVertex(unsigned int id, vertex *vertex)
{
    vertex->count = 0;
    vertex->id = id;
    vertex->next = NULL;
    vertex->neigbhors = (unsigned int *)malloc(2 * vertexSize * sizeof(unsigned int));
}

void deallocateVertex(vertex *vertex)
{
    extention *next = NULL;
    extention *current = NULL;
    if (vertex->next != NULL)
    {
        current = vertex->next;
        free(current);
        next = current->next;
    }
    while (next != NULL)
    {
        current = next;
        next = current->next;
        free(current);
    }
    free(vertex);
}

void extend(vertex *v)
{
    extention *last = (extention *)malloc(sizeof(extention));
    last->count = 0;
    last->neigbhors = (unsigned int *)malloc(2 * vertexSize * sizeof(unsigned int));
    last->next == NULL;
    if (v->next == NULL)
    {
        v->next = last;
        last->id = 1;
    }
    else
    {
        v->last->next = last;
        last->id = v->last->id + 1;
    }
    v->last = last;
}

void addEdge(unsigned int id, vertex *v, unsigned int weight)
{
    if (v->count < vertexSize)
    {
        v->neigbhors[2 * v->count] = id;
        v->neigbhors[2 * v->count + 1] = weight;
        v->count++;
        return;
    }

    if (v->last == NULL || v->last->count == vertexSize)
    {
        extend(v);
    }
    if (v->last == NULL)
    {
        printf("first extend error in add Edge\n");
        exit(0);
    }
    if (v->last->count == vertexSize)
    {
        printf("secondary extend error in add Edge\n");
        exit(0);
    }
    v->last->neigbhors[2 * v->last->count] = id;
    v->last->neigbhors[2 * v->last->count + 1] = weight;
    v->last->count++;
  
}

void printVertex(vertex *v)
{
    int count = v->count;
    if (v->last != NULL)
    {
        count = v->last->id * vertexSize + v->last->count;
    }
    printf("id: %u count:%d list:\n", v->id, count);
    for (int i = 0; i < v->count; i++)
    {
        printf(GRN "%u " RESET, v->neigbhors[2 * i]);
            printf(RED "%u " RESET, v->neigbhors[2 * i + 1]);
    }
    extention *next = v->next;
    while (next != NULL)
    {
        for (int i = 0; i < next->count; i++)
        {
            printf(GRN "%u " RESET, next->neigbhors[2 * i]);
            printf(RED "%u " RESET, next->neigbhors[2 * i + 1]);
        }
        next = next->next;
    }
    printf("\n");
}

typedef struct vertexBuffer
{
    struct vertexBuffer *next;
    vertex *vertices;
    int id;
} vertexBuffer;

vertexBuffer *extendBuffer(vertexBuffer *buffer)
{

    vertexBuffer *res = (vertexBuffer *)malloc(sizeof(vertexBuffer));
    res->vertices = (vertex *)malloc(sizeof(vertex) * vertexBufferSize);
    res->next = NULL;

    if (buffer != NULL)
    {
        buffer->next = res;
        res->id = buffer->id + 1;
    }
    else
    {
        res->id = 0;
    }
    // printf("allocated block with id %d\n", res->id);
    for (int i = 0; i < vertexBufferSize; i++)
    {
        unsigned int id = i + vertexBufferSize * res->id;
        allocateVertex(id, &res->vertices[i]);
        // printf("allocated %lu with id %lu\n", id, res->vertices[i].id);
    }
    return res;
}

void deallocateVertexBuffer(vertexBuffer *buffer)
{
    vertexBuffer *current, *next;
    current = buffer;
    do
    {
        next = current->next;
        for (int i = 0; i < vertexBufferSize; i++)
        {
            deallocateVertex(&current->vertices[i]);
        }
        free(current->vertices);
        free(current);
    } while (next != NULL);
}

vertex *getVertex(vertexBuffer *buffer, unsigned int id)
{
    unsigned int pos = id & (vertexBufferSize - 1);

    int bufferId = id / vertexBufferSize;
    vertexBuffer *current = buffer;
    while (current->id < bufferId)
    {

        if (current->next == NULL)
        {
            vertexBuffer *temp;
            current = extendBuffer(current);
            continue;
        }

        current = current->next;
    }
    vertex *res = &(current->vertices[pos]);
    if (res->id != id)
    {
        printf("get vertex id, error given %u but vertex had %u\n", id, res->id);
        printf("Current block id %d, and target is %d\n", current->id, bufferId);
        exit(-1);
    }
    return res;
}

int main(int argc, char *argv[])
{
    unsigned int defaultWeight;

    if (argc == 3)
    {
        
        int read = sscanf(argv[2], "%u", &defaultWeight);
        if(read == 0)
        {
            defaultWeight = 1;
        }
    }
    else
    {
        return -1;
    }

    vertexBuffer *graph = extendBuffer(NULL);

    char *buffer = (char *)calloc(256, sizeof(char));
    FILE *fd = fopen(argv[1], "r");
    if(fd == NULL)
    {
        printf("No such file exists");
        return -1;
    }
    int counter = 0;
    int edgesCounter = 0;
    char last;
    while (last = fgetc(fd))
    {
        if (last == EOF)
        {
            counter = 0;
            unsigned int v1, v2, w;
            int numberRead = sscanf(buffer, "%u %u", &v1, &v2);
            if (numberRead == 2)
            {
                w = defaultWeight;
            }
            if (numberRead == 2 || numberRead == 3)
            {
                edgesCounter++;
                addEdge(v2, getVertex(graph, v1), w);
                addEdge(v1, getVertex(graph, v2), w);
            }
            break;
        }

        buffer[counter] = last;
        counter++;

        if (last == '\n')
        {
            counter = 0;
            unsigned int v1, v2, w;
            int numberRead = sscanf(buffer, "%u %u %u", &v1, &v2, &w);

            if (numberRead == 2)
            {
                w = defaultWeight;
            }
            if (numberRead == 2 || numberRead == 3)
            {
                edgesCounter++;
                addEdge(v2, getVertex(graph, v1), w);
                addEdge(v1, getVertex(graph, v2), w);
            }
        }
    }

    fclose(fd);
    while (1)
    {
        unsigned int id;
        printf("Give an id to print the vertex\n");
        scanf("%u", &id);
        printVertex(getVertex(graph, id));
    }
}