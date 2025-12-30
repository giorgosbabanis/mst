#include <stdio.h>
#include "matrix_market.h"
#include <stdlib.h>
#include <parallel_io.h>

void printCSR(int* row_ptr, int* col_idx, int* values, int m){
    for(int r = 0; r < m; r++){
        for(int i = row_ptr[r]; i < row_ptr[r+1]; i++){
            printf("%d %d %d\n",r,col_idx[i], values[i]);
        }
    }
}
void printMTX_Graph(struct Matrix_Market* mtx){
    if(!mtx){
        printf("The graph is null\n"); 
        return;
    }
    for(int i = 0; i < mtx->nnz; i++){ 
        printf("(%d,%d) : %d , index: %d\n", mtx->R[i], mtx->C[i], mtx->V[i], i);
    }
    printf("Vertices : %ld , Edges : %ld \n", mtx->m, mtx->nnz);
}


int main(){
    char *filename = "../data_sample/graph10K.gr";

    struct File_Atoms * A;
    A = (typeof(A)) malloc(sizeof(*A));
    file_to_lines(A, filename, 0);

    struct Matrix_Market * graph = mtx_read_graph(A);
    //printMTX_Graph(graph);

    int *row_ptr = (int *)malloc((graph->m + 1) * sizeof(int));
    int *col_idx = (int *)malloc((graph->nnz) * sizeof(int));
    int *values = (int *)malloc((graph->nnz) * sizeof(int));
    printMTX_Graph(graph);
}