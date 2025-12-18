#include <stdio.h>
#include "storage_formats/matrix_market/matrix_market.h"
#include <stdlib.h>

#define INT_T int
#define ValueType double
#include "aux/csr_converter.c"


void printMTX(struct Matrix_Market* mtx){
    for(int i = 0; i < mtx->nnz; i++){
        double* vals = (double*)mtx->V; 
        printf("(%d,%d) : %f , index: %d\n", mtx->R[i], mtx->C[i], vals[i], i);
    }
    printf("nnz is: %ld\n", mtx->nnz);
    printf("nnz sym is: %ld\n", mtx->nnz_sym);
    printf("nnz diag is: %ld\n", mtx->nnz_diag);
    printf("nnz non diag is: %ld\n", mtx->nnz_non_diag);
}

void printCSR(int* row_ptr, int* col_idx, double* values, int m){
    for(int r = 0; r < m; r++){
        for(int i = row_ptr[r]; i < row_ptr[r+1]; i++){
            printf("%d %d %f\n",r,col_idx[i], values[i]);
        }
    }
}


int main(){
    struct Matrix_Market *graph = mtx_new();
    char *filename = "/home/giorgos/Downloads/494_bus/494_bus.mtx";

    graph = mtx_read(filename, 1, 0);
    //printMTX(graph);

    int* row_ptr = (int*)malloc((graph->m + 1) * sizeof(int));
    int* col_idx = (int*)malloc((graph->nnz) * sizeof(int));
    double* values = (double*)malloc((graph->nnz) * sizeof(double));

    coo_to_csr(graph->R, graph->C, graph->V, graph->m, graph->n, graph->nnz, row_ptr, col_idx, values, 1, 0);

    printCSR(row_ptr, col_idx, values, graph->m);
}