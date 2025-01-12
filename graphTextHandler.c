#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct vertex
{
    unsigned long id;
    int count;
    unsigned long* neigbhors;
    struct extention* next;
};

struct extention
{
    struct vertex* first;
    struct extention* next;
    unsigned long* neigbhors;
};

struct vertex* allocateVertex(int id)
{
    struct vertex* res = (struct vertex*)malloc(sizeof(struct vertex));
    
    res->count = 0;
    res->id = id;
    res->next=NULL;
    res->neigbhors = (unsigned long*)malloc(16*sizeof(unsigned long));

    return res;
}

struct extention* extend(struct vertex* v)
{
    struct extention* res = (struct extention*)malloc(sizeof(struct extention));
    res->first = v;
    res->next = NULL;
    res->neigbhors = (unsigned long*)malloc(16*sizeof(unsigned long));

    return res;
}

void addEdge(unsigned long id, struct vertex* v)
{
    int pos = v->count&15;
    if(v->count<16)
    {
        v->neigbhors[pos] = id;
    }
    else
    {
        printf("TOO MANY NEIGHBORS");
        exit(-1);
    }
    return;
    
    if( v->count&15 == 0)
    {

    }
}

void addEdge(unsigned long id, unsigned long weight, struct vertex* v)
{

}

int main(int argc, char *argv[])
{
    if(argc != 3){
        return 0;
    }

    
   
    char* buffer = (char*)calloc(256,sizeof(char));
    FILE* fd = fopen(argv[1],"r");
    int counter = 0;
    int edgesCounter = 0;
    int capacity = 1000;
    char last;
    while(last = fgetc(fd)!= EOF)
    {
        buffer[counter] = last;
        if(last == '\n')
        {
            edgesCounter++;
            
        }
    }
    printf("%d\n",edgesCounter);
}