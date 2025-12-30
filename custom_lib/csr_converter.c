#include "csr_converter.h"
#include "stdlib.h"
#include "stdio.h"
#include "omp.h"

struct csr* new_csr(){
    struct csr* csr;
    csr = (typeof(csr)) malloc(sizeof(*csr));
    csr->edges_count = 0;
    csr->vertices_count = 0;
    csr->offsets_list = NULL;
    csr->edges_list = NULL;
    return csr;
}

void csr_clean(struct csr* csr){
    if(csr == NULL){
        return;
    }
    free(csr->offsets_list);
    free(csr->edges_list);
}
struct csr* coo_to_csr(struct Matrix_Market *mtx, int add_symmetric_edges){
    
    struct csr* csr = new_csr();
    long nnz = mtx->nnz;    
    long vertices_count = mtx->m;
    csr->edges_count = nnz;
    if(add_symmetric_edges){
        csr->edges_count *= 2;
    }
    csr->vertices_count = vertices_count;
    csr->offsets_list = (long*)malloc((vertices_count + 1) * sizeof(long));
    csr->edges_list = (int*)malloc((csr->edges_count * 2) * sizeof(int));
    int* vertex_offsets = (int*)malloc((vertices_count + 1) * sizeof(int));

    
    //Calculate permutations and row pointers
    int threads = omp_get_max_threads();
    long * local_offsets = (long *)malloc(threads * sizeof(long));

    #pragma omp parallel shared(local_offsets) shared(csr)
    {
        int id = omp_get_thread_num();
        
        
        #pragma omp for
        for(int i = 0; i < vertices_count + 1; i++){
            csr->offsets_list[i] = 0;
            vertex_offsets = 0;
        }

        #pragma omp for
        for(long i = 0; i < nnz; i++){
            unsigned int v1 = mtx->R[i];
            __atomic_fetch_add(&csr->offsets_list[v1], 1, __ATOMIC_RELAXED);
            if(add_symmetric_edges){
                unsigned int v2 = mtx->C[i];
                __atomic_fetch_add(&csr->offsets_list[v2], 1, __ATOMIC_RELAXED);
            }
        }
    
    //Scan reduce the offsets
    
        int start = id * (vertices_count + 1) / threads;
        int end = (id + 1) * (vertices_count + 1) / threads;

        long offset = 0;
        for(int i = start; i < end; i++){
            offset += csr->offsets_list[i];
        }
        
        local_offsets[id] = offset;

        #pragma omp barrier

        #pragma omp single
        {
            for(int i = 1; i < threads; i++){
                local_offsets[i] += local_offsets[i - 1];
            }
        }

        offset = (id == 0)? 0 : local_offsets[id - 1];
        csr->offsets_list[start] += offset;
        for(int i = start + 1; i < end; i++){
            csr->offsets_list[i] += csr->offsets_list[i - 1];
        }

        long start_edge = id * nnz / threads;
        long end_edge = (id + 1) * nnz / threads;

        for(long i = start_edge; i < end_edge; i++){
            unsigned int v1 = mtx->R[i];
            unsigned int v2 = mtx->C[i];
            unsigned int w = mtx->V[i];

            long pos = csr->offsets_list[v1 - 1] + __atomic_fetch_add(&vertex_offsets[v1], 1, __ATOMIC_RELAXED);
            csr->edges_list[2 * pos] = v2;
            csr->edges_list[2 * pos + 1] = w;

            if(add_symmetric_edges){
                long pos = csr->offsets_list[v2 - 1] + __atomic_fetch_add(&vertex_offsets[v2], 1, __ATOMIC_RELAXED);
                csr->edges_list[2 * pos] = v1;
                csr->edges_list[2 * pos + 1] = w;
            }
        }
    }
}

