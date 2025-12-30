#ifndef MATRIX_MARKET_H
#define MATRIX_MARKET_H

#include "parallel_io.h"
#include <complex.h>


/* Matrix in COO format.
 *
 * format : array (dense) or coordinate (COO)
 * field  : weight type -> real, integer, complex, pattern (none)
 *
 * symmetric          : Matrix has some kind of symmetry.
 * standard_symmetry  : Equal to its transpose matrix.
 * skew_symmetry      : Equal to the negative of its transpose matrix.
 *                      The elements of the main diagonal are all zero.
 * hermitian_symmetry : Equal to the conjugate transpose.
 * symmetry_expanded  : Whether the symmetric elements are included or not.
 *                      Always TRUE for general matrices.
 *
 * m   : num rows
 * n   : num columns
 * nnz : num of non-zeros
 *
 * R : row indexes
 * C : column indexes
 * V : values
 */
struct Matrix_Market {
	char * filename;

	char * format;
	char * field;

	int symmetric;
	int standard_symmetry;
	int skew_symmetry;
	int hermitian_symmetry;
	int symmetry_expanded;

	long m;
	long n;
	long nnz;
	long nnz_sym;

	long nnz_diag;
	long nnz_non_diag;

	unsigned int * R;
	unsigned int * C;

	unsigned int * V;
};


void mtx_init(struct Matrix_Market * obj);
struct Matrix_Market * mtx_new();
void mtx_clean(struct Matrix_Market * obj);
void mtx_destroy(struct Matrix_Market ** obj_ptr);

struct Matrix_Market * mtx_read_graph(struct File_Atoms * A);

#endif /* MATRIX_MARKET_H */

