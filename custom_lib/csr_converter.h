#include "matrix_market.h"

struct csr{
    unsigned long vertices_count;
	unsigned long edges_count;
	unsigned long* offsets_list;
	unsigned int* edges_list;
};

struct csr* new_csr();

void csr_clean(struct csr* csr);

void coo_to_csr(struct Matrix_Market *mtx, struct csr *csr);