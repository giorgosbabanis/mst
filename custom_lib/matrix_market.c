#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <complex.h>
#include <tgmath.h>

#include "io.h"
#include "parallel_io.h"

#include "matrix_market.h"

void
mtx_init(struct Matrix_Market * MTX)
{
	MTX->filename = NULL;
	MTX->format = NULL;
	MTX->field = NULL;
	MTX->R = NULL;
	MTX->C = NULL;
	MTX->V = NULL;
}


struct Matrix_Market *
mtx_new()
{
	struct Matrix_Market * MTX;
	MTX = (typeof(MTX)) malloc(sizeof(*MTX));
	mtx_init(MTX);
	return MTX;
}


void
mtx_clean(struct Matrix_Market * MTX)
{
	if (MTX == NULL)
		return;
	free(MTX->filename);
	free(MTX->format);
	free(MTX->field);
	free(MTX->R);
	free(MTX->C);
	free(MTX->V);
}


void
mtx_destroy(struct Matrix_Market ** MTX_ptr)
{
	if (MTX_ptr == NULL)
		return;
	mtx_clean(*MTX_ptr);
	free(*MTX_ptr);
	*MTX_ptr = NULL;
}


struct Matrix_Market *
mtx_read_graph(struct File_Atoms * A){
	struct Matrix_Market * graph = mtx_new();
	long index = 0;
	graph->m = 0;
	graph->nnz = 0;

	for(;index < A->num_atoms; index++){
		char * line = A->atoms[index];

		int m,nnz;
		if(A->atom_len[index] >= 4 && strncmp(line, "p sp", 4) == 0){
			
			int res = sscanf(line,"p sp %d %d", &m, &nnz);
			if(res == 2){
				graph->m = m;
				graph->n = m;
				graph->nnz = nnz;
				break;
			}
			else{
				printf("Error reading the line with the number of vertices and edges on the following line\n");
				printf("%s\n", line);
			    exit(1);
			}
		}
	}

	index++;

	if(graph->m <= 0){
		printf("Error, did not read the number of vertices\n");
		exit(1);
	}

	if(graph->nnz <= 0){
		printf("Error, did not read number of edges\n");
		exit(1);
	}

	graph->R = malloc(graph->nnz * sizeof(unsigned int));
	graph->C = malloc(graph->nnz * sizeof(unsigned int));
	graph->V = malloc(graph->nnz * sizeof(int));
	
	#pragma omp parallel
	{
		int threads = omp_get_num_threads();
		int id = omp_get_thread_num();

		long start = graph->nnz * id / threads ; 
		long end = graph->nnz * (id + 1) / threads;

		// usleep(id * 1000);
		// printf("I got id %d, start %ld and end %ld\n", id, start, end);
		unsigned int *V = graph->V;

		for(long i = start; i < end; i++){
			char * line = A->atoms[index + i];
			int a, b, w; 

			int res = sscanf(line, "a %d %d %d", &a, &b, &w);
			if(res == 3){

				graph->R[i] = a;
				graph->C[i] = b;
				V[i] = w;
			}
			else{
				printf("Problem reading on line: %s\n", line);
			}
		}
	}
	
	return graph;
}

