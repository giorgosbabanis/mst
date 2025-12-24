#include <stdio.h>
#include "storage_formats/matrix_market/matrix_market.h"
#include <stdlib.h>
#include <parallel_io.h>

#define INT_T int
#define ValueType int
#include "aux/csr_converter.c"


void printMTX(struct Matrix_Market* mtx){
    if(!mtx){
        printf("The mtx is null\n");
        return;
    }
    for(int i = 0; i < mtx->nnz; i++){
        double* vals = (double*)mtx->V; 
        printf("(%d,%d) : %f , index: %d\n", mtx->R[i], mtx->C[i], vals[i], i);
    }
    printf("nnz is: %ld\n", mtx->nnz);
    printf("nnz sym is: %ld\n", mtx->nnz_sym);
    printf("nnz diag is: %ld\n", mtx->nnz_diag);
    printf("nnz non diag is: %ld\n", mtx->nnz_non_diag);
}

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
        int* vals = (int*)mtx->V; 
        printf("(%d,%d) : %d , index: %d\n", mtx->R[i], mtx->C[i], vals[i], i);
    }
    printf("Vertices : %d , Edges : %d \n", mtx->m, mtx->nnz);
}


int main(){
    char *filename = "/home/giorgos/LaganLighter/data/graph10K.gr";

    int normal_read = 0;
    if (normal_read)
    {
        struct Matrix_Market * graph = mtx_read(filename, 1, 0);
        // printMTX(graph);

        int *row_ptr = (int *)malloc((graph->m + 1) * sizeof(int));
        int *col_idx = (int *)malloc((graph->nnz) * sizeof(int));
        double *values = (double *)malloc((graph->nnz) * sizeof(double));

        coo_to_csr(graph->R, graph->C, graph->V, graph->m, graph->n, graph->nnz, row_ptr, col_idx, values, 1, 0);

        printCSR(row_ptr, col_idx, values, graph->m);
    }
    else{
        struct File_Atoms * A;
        A = (typeof(A)) malloc(sizeof(*A));
        file_to_lines(A, filename, 0);

        struct Matrix_Market * graph = mtx_read_graph(A);
        //printMTX_Graph(graph);

        long *row_ptr = (long *)malloc((graph->m + 1) * sizeof(long));
        int *col_idx = (int *)malloc((graph->nnz) * sizeof(int));
        int *values = (int *)malloc((graph->nnz) * sizeof(int));

        coo_to_csr(graph->R, graph->C, graph->V, graph->m, graph->n, graph->nnz, row_ptr, col_idx, values, 0, 0);

        printCSR(row_ptr, col_idx, values, graph->m);
    }
}