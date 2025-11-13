#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>

#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define RESET "\x1B[0m"

const int vertexSize = 128;
const unsigned int vertexBufferSize = 16384;

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
    vertex->last = NULL;
    vertex->neigbhors = (unsigned int *)malloc(2 * vertexSize * sizeof(unsigned int));
}

void deallocateVertex(vertex *vertex)
{
    extention *next = NULL;
    extention *current = vertex->next;
    free(vertex->neigbhors);
    // free(vertex);
    if (current == NULL)
    {
        return;
    }
    while (next != NULL)
    {
        next = current->next;
        free(current->neigbhors);
        free(current);
        current = next;
    }
}

void deallocateExtentions(vertex *vertex)
{
    extention *next = NULL;
    extention *current = vertex->next;
    if (current == NULL)
    {
        return;
    }
    while (next != NULL)
    {
        next = current->next;
        free(current->neigbhors);
        free(current);
        current = next;
    }
}

extention* extend(vertex *v)
{
    //printf("extending\n");
    extention *last = (extention *)malloc(sizeof(extention));
    last->count = 0;
    last->neigbhors = (unsigned int *)malloc(2 * vertexSize * sizeof(unsigned int));
    last->next = NULL;
    if (v->next == NULL)
    {
        //printf("ok\n");
        v->next = last;
        last->id = 1;
    }
    else
    {
        if(v->last == NULL){
            printf("error in extend, expected last not to be null\n");
            exit(1);
        }
        v->last->next = last;
        last->id = v->last->id + 1;
    }
    //printf("extended with id %d\n", last->id);
    return last;
}

void addEdge(unsigned int id, vertex *v, unsigned int weight)
{
    //printf("entered add edge with id %u and vertex %u and count %d\n", id, v->id, v->count);
    // if(id > 1000 || v->id > 1000){
    //     return;
    // }
    if (v->count < vertexSize)
    {
        //printf("in first if with count %d\n", v->count);
        v->neigbhors[2 * v->count] = id;
        v->neigbhors[2 * v->count + 1] = weight;
        v->count++;
        return;
    }
    //printf("past if 1\n");
    if(v->last == NULL){
        v->last = extend(v);
    }
  
    //printf("past if 2 with id %u and count %d\n", v->id, v->count);
    int g = v->last->id;
    //printf("%u \n", g);
    //printf("past if 2 with count %d and id %u\n", v->last->count, id);
    if(v->last->count < vertexSize){
        v->last->neigbhors[2 * v->last->count] = id;
        v->last->neigbhors[2 * v->last->count + 1] = weight;
        v->last->count++;
        //printf("about to return\n");
        return;
    }
    //printf("past if 3\n");
    v->last = extend(v);
    //printf("past extend\n");
    v->last->neigbhors[2 * v->last->count] = id;
    //printf("past id\n");
    v->last->neigbhors[2 * v->last->count + 1] = weight;
    //printf("past weight\n");
    v->last->count++;
    //printf("past everything\n");
    // return;
    // if (v->last == NULL || v->last->count > vertexSize - 1)
    // {
    //     // printf("before extend\n");
    //     // printf("trying to add %u %u %u\n", id, v->id, weight);
    //     extend(v);
    //     //printf("after extend\n");
    // }
    // if (v->last == NULL)
    // {
    //     printf("first extend error in add Edge\n");
    //     exit(0);
    // }
    // if (v->last->count > vertexSize - 1)
    // {
    //     printf("secondary extend error in add Edge\n");
    //     exit(0);
    // }
    // v->last->neigbhors[2 * v->last->count] = id;
    // v->last->neigbhors[2 * v->last->count + 1] = weight;
    // v->last->count++;
    // // printf("ok\n");
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

    //printf("extending with id %d\n", res->id);
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
        //printf("buffer id %d id %u\n", current->id, id);
        if (current->next == NULL)
        {
            //printf("before extend\n");
            extendBuffer(current);
            //printf("after extend\n");
        }
        if(current->next == NULL){
            printf("extend buffer failed in get vertex for id %d ", id);
            exit(-1);
        }
        //printf("before next\n");
        current = current->next;
        //printf("after next\n");
    }
    //printf("exited loop with id %u and pos %u\n", id, pos);
    vertex *res = &(current->vertices[pos]);
    //printf("after equal\n");
    if (res->id != id)
    {
        printf("get vertex id, error given %u but vertex had %u\n", id, res->id);
        printf("Current block id %d, and target is %d\n", current->id, bufferId);
        exit(-1);
    }
    //printf("after equal check\n");
    return res;
}

void convertVertex(vertex *v)
{
    int count = v->count;
    if (count == 0)
    {
        return;
    }
    if (v->last != NULL)
    {
        count = v->last->id * vertexSize + v->last->count;
    }

    unsigned int *buffer = (unsigned int *)malloc(sizeof(unsigned int) * count * 2);
    int counter = 0;
    memcpy(buffer, v->neigbhors, v->count * 2 * sizeof(unsigned int));
    counter += v->count * 2;
    extention *current = v->next;
    while (current != NULL)
    {
        memcpy(&buffer[counter], current->neigbhors, current->count * 2 * sizeof(unsigned int));
        counter += current->count * 2;
        current = current->next;
    }
    deallocateExtentions(v);
    v->next = NULL;
    v->last = NULL;
    v->count = count;
    free(v->neigbhors);
    v->neigbhors = buffer;
}

void swap(unsigned int *a, unsigned int *b)
{
    unsigned int wa = a[1];
    unsigned int t = a[0];
    a[0] = b[0];
    a[1] = b[1];
    b[0] = t;
    b[1] = wa;
}

void bubbleSort(unsigned int *array, int low, int high)
{
    char flag = 0;
    do
    {
        flag = 0;
        for (int i = low; i < high - 1; i++)
        {
            if (array[2 * i] > array[2 * i + 2])
            {
                flag = 1;
                swap(&array[2 * i], &array[2 * i + 2]);
            }
        }
    } while (flag);
}

int partition(unsigned int *array, int low, int high)
{
    unsigned int pivot = array[2 * high];
    unsigned int greater = low - 1;

    for (unsigned int i = low; i < high; i++)
    {
        if (array[2 * i] < pivot)
        {
            greater++;
            swap(&array[2 * i], &array[2 * greater]);
        }
    }

    swap(&array[2 * greater + 2], &array[2 * high]);
    return greater + 1;
}

void quickShort(unsigned int *array, int low, int high) // Replace quicksort
{
    // printf("%u %u\n", low, high);
    if (low < high)
    {
        unsigned int pivot = partition(array, low, high);

        quickShort(array, low, pivot - 1);

        quickShort(array, pivot + 1, high);
    }
    // else
    // {
    //     bubbleSort(array, low, high);
    // }
}

void eliminateDoubles(vertex *v)
{
    if (v->count == 0)
    {
        return;
    }
    if (v->next != NULL)
    {
        printf("Convert vertex before eliminating doubles");
        return;
    }

    unsigned int current = v->neigbhors[0];
    unsigned int currentWeight = v->neigbhors[1];
    unsigned int counter = 0;

    for (int i = 1; i < v->count; i++)
    {
        if (current != v->neigbhors[2 * i])
        {
            v->neigbhors[2 * counter] = current;
            v->neigbhors[2 * counter + 1] = currentWeight;
            counter++;
            current = v->neigbhors[2 * i];
            currentWeight = v->neigbhors[2 * i + 1];
        }
        else
        {
            if (currentWeight > v->neigbhors[2 * i + 1])
            {
                currentWeight = v->neigbhors[2 * i + 1];
            }
        }
    }

    v->neigbhors[2 * counter] = current;
    v->neigbhors[2 * counter + 1] = currentWeight;
    counter++;
    v->count = counter;
}

void printFromList(unsigned long *offsetList, unsigned int *edgeList, unsigned int id)
{
    unsigned long start = offsetList[id];
    unsigned long end = offsetList[id + 1];
    printf("id: %u count:%d list:\n", id, (int)(end - start));
    for (int i = start; i < end; i++)
    {
        printf(GRN "%u " RESET, edgeList[2 * i]);
        printf(RED "%u " RESET, edgeList[2 * i + 1]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    unsigned int defaultWeight;
    int mode = 0;
    if (argc == 4 || argc == 5)
    {

        int read = sscanf(argv[2], "%u", &defaultWeight);
        if (read == 0)
        {
            defaultWeight = 1;
        }
    }
    else
    {
        return -1;
    }
    if (argc == 5)
    {
        sscanf(argv[4], "%d", &mode);
    }

    vertexBuffer *graph = extendBuffer(NULL);

    char *buffer = (char *)calloc(256, sizeof(char));
    FILE *fd = fopen(argv[1], "r");
    if (fd == NULL)
    {
        printf("No such file exists");
        return -1;
    }

    int counter = 0;
    unsigned long maxVertex = 0;
    char last;
    unsigned long edgeCounter = 0;

    while (last = fgetc(fd))
    {
        if (last == EOF)
        {
            if (counter == 0)
            {
                break;
            }
            counter = 0;
            unsigned int v1, v2, w;
            int numberRead = 0;
            if (mode == 1)
            {
                char a = ' ';
                numberRead = sscanf(buffer, "%c %u %u %u", &a, &v1, &v2, &w);
                if (a != 'a')
                {
                    numberRead = 0;
                }
            }
            else
            {
                numberRead = sscanf(buffer, "%u %u %u", &v1, &v2, &w);
            }
            if (numberRead == 2)
            {
                w = defaultWeight;
            }
            if (numberRead == 2 || numberRead == 3 || numberRead == 4)
            {
                
                addEdge(v2, getVertex(graph, v1), w);
                addEdge(v1, getVertex(graph, v2), w);
                if (maxVertex < v1)
                {
                    maxVertex = v1;
                }
                if (maxVertex < v2)
                {
                    maxVertex = v2;
                }
                // printf("%u %u %u\n", v1, v2, w);
            }
            break;
        }

        buffer[counter] = last;
        counter++;

        if (last == '\n' || last == EOF)
        {
            unsigned int v1, v2, w;
            int numberRead = 0;
            if (mode == 1)
            {
                char a = 'b';
                numberRead = sscanf(buffer, "%c %u %u %u", &a, &v1, &v2, &w);
                if (a != 'a')
                {
                    numberRead = 0;
                }
            }
            else
            {
                numberRead = sscanf(buffer, "%u %u %u", &v1, &v2, &w);
            }

            if (numberRead == 2)
            {
                w = defaultWeight;
            }
            if (numberRead == 2 || numberRead == 3 || numberRead == 4)
            {
                addEdge(v2, getVertex(graph, v1), w);
                //printf("exited add edge\n");
                addEdge(v1, getVertex(graph, v2), w);
                //printf("exited add edge\n");
                if (maxVertex < v1)
                {
                    maxVertex = v1;
                }
                if (maxVertex < v2)
                {
                    maxVertex = v2;
                }
            }
            memset(buffer, 0, counter * sizeof(char));
            counter = 0;
            edgeCounter++;
            if((edgeCounter & 131071) == 0){
                printf("Read edge %lu\n", edgeCounter);
            }
        }
        // if(last == EOF){
        //     break;
        // }
    }

    fclose(fd);
    unsigned long edgesCount = 0;

    printf("finished reading\n");

    for (unsigned int i = 0; i <= maxVertex; i++)
    {
        vertex *v = getVertex(graph, i);
        if (i == 0)
        {
            printf("%d\n", v->count);
        }
        convertVertex(v);
        bubbleSort(v->neigbhors, 0, v->count);
        eliminateDoubles(v);
        edgesCount += v->count;
        if((i & 16383) == 0){
                printf("Sorted vertex %u\n", i);
            }
    }

    printf("finished sorting and eliminating\n");
    // for (int i = 0; i <= maxVertex; i++)
    // {
    //     vertex *v = getVertex(graph, i);
    //     bubbleSort(v->neigbhors, 0, v->count);
    // }

    // for (int i = 0; i <= maxVertex; i++)
    // {
    //     vertex *v = getVertex(graph, i);
    //     eliminateDoubles(v);
    // }

    // while (1)
    // {
    //     unsigned int id, scanned;
    //     char op;
    //     printf("Give an id to print\n");
    //     scanned = scanf("%c %u", &op, &id);
    //     if (op == 'p')
    //     {
    //         printVertex(getVertex(graph, id));
    //     }
    //     // if(op == 'c')
    //     // {
    //     //     convertVertex(getVertex(graph, id));
    //     // }
    //     // if (op == 's')
    //     // {
    //     //     vertex *v = getVertex(graph,id);
    //     //     quickShort(v->neigbhors, 0, v->count);
    //     //     //eliminateDoubles(getVertex(graph, id));
    //     //     printVertex(getVertex(graph, id));
    //     // }
    //     if (op == 'x')
    //     {
    //         break;
    //     }
    // }

    size_t graphSize = sizeof(unsigned long) * (2 + maxVertex + 2) + sizeof(unsigned int) * (edgesCount * 2);

    unsigned long *graphMemory = (unsigned long *)(malloc(graphSize));
    graphMemory[0] = maxVertex + 1;
    graphMemory[1] = edgesCount;
    unsigned long *offsetList = &graphMemory[2];
    unsigned int *edgesList = (unsigned int *)&offsetList[maxVertex + 2];

    unsigned long c = 0;
    for (unsigned int i = 0; i <= maxVertex; i++)
    {
        vertex *v = getVertex(graph, i);
        offsetList[i] = c;
        if (v->count != 0)
        {
            memcpy(&edgesList[c * 2], v->neigbhors, 2 * sizeof(unsigned int) * v->count);
            c += v->count;
        }
        deallocateVertex(v);
    }

    // deallocateVertexBuffer(graph);

    offsetList[maxVertex + 1] = c;

    printf("finished creating graph\n");

    printf("vertices %lu edges %lu \n", graphMemory[0], graphMemory[1]);

    
    // while (1)
    // {
    //     unsigned int id, scanned;
    //     char op;
    //     printf("Give an id to print from new graph\n");
    //     scanned = scanf("%c %u", &op, &id);
    //     if (op == 'p')
    //     {
    //         printFromList(offsetList, edgesList, id);
    //     }

    //     if (op == 'x')
    //     {
    //         break;
    //     }
    // }
  
    
    
    int fdm = open(argv[3], O_CREAT | O_RDWR | O_TRUNC, 0666);
    int ret = ftruncate(fdm, graphSize);
    if (ret < 0)
    {
        perror("ftruncate"); // will print descriptive error
        printf("Error code: %d (%s)\n", errno, strerror(errno));
    }
    unsigned long *map = (unsigned long *)mmap(NULL, graphSize, PROT_READ | PROT_WRITE, MAP_SHARED, fdm, 0);
    memcpy(map, graphMemory, graphSize);
    ret = msync(map, graphSize, MS_SYNC);
    if (ret < 0)
    {
        perror("msync");
        printf("Error code: %d (%s)\n", errno, strerror(errno));
        printf("Mapping failed with %d\n", ret);
    }
    else
    {
        printf("OK\n");
    }
    close(fdm);
    
}