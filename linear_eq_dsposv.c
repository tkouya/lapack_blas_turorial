/*************************************************/
/* LAPACK/BLAS Tutorial                          */
/* Solver for Linear equation with DSPOSV        */
/* Last Update: 2016-11-30 (Wed) T.Kouya         */
/*************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lapacke.h"
#include "cblas.h"

int main()
{
	lapack_int i, j, dim, itimes;
	lapack_int inc_vec_x, inc_vec_b;
	lapack_int info;

	double *mat_a, *vec_b, *vec_x, *vec_x_iter;
	double alpha, beta;
	double running_time;

	// input dimension of linear equation to be solved
	printf("Dim = "); scanf("%d", &dim);

	if(dim <= 0)
	{
		printf("Illegal dimension! (dim = %d)\n", dim);
		return EXIT_FAILURE;
	}

	// initialize a matrix and vectors
	mat_a = (double *)calloc(dim * dim, sizeof(double));
	vec_x = (double *)calloc(dim, sizeof(double));
	vec_x_iter = (double *)calloc(dim, sizeof(double));
	vec_b = (double *)calloc(dim, sizeof(double));

	// input mat_a and vec_x
	for(i = 0; i < dim; i++)
	{
		for(j = 0; j < dim; j++)
		{
			//mat_a[i * dim + j] = (double)rand() / (double)RAND_MAX;
			mat_a[i * dim + j] = 1.0 / (double)(i + j + 1);
			if((i + j + 1) % 2 != 0)
				mat_a[i * dim + j] = -mat_a[i * dim + j];
		}
		mat_a[i * dim + i] += 2.0;
		vec_x[i] = 1.0 / (double)(i + 1);
	}

	// size(vec_x) == size(vec_b)
	inc_vec_x = inc_vec_b = 1;

	// vec_b := 1.0 * mat_a * vec_x + 0.0 * vec_b
	alpha = 1.0;
	beta = 0.0;
	cblas_dsymv(CblasRowMajor, CblasUpper, dim, alpha, mat_a, dim, vec_x, inc_vec_x, beta, vec_b, inc_vec_b);

	// print
	for(i = 0; i < dim; i++)
	{
		printf("[");
		for(j = 0; j < dim; j++)
			printf("%10.3f ", mat_a[i * dim + j]);
		printf("]  %10.3f = %10.3f\n", vec_x[i], vec_b[i]);
	}

	// solve A * X = C -> C := X
	info = LAPACKE_dsposv(LAPACK_ROW_MAJOR, 'U', dim, 1, mat_a, dim, vec_b, 1, vec_x_iter, 1, &itimes);
	printf("DSPOSV info = %d, Iterative Times = %d\n", info, itimes);

	// print
	printf("calculated x = \n");
	for(i = 0; i < dim; i++)
	{
		//printf("%3d -> %3d: ", i, pivot[i]);
		printf("%25.17e ", vec_x_iter[i]);
		printf("\n");
	}

	// diff
	printf("x - calculated x = \n");
	for(i = 0; i < dim; i++)
	{
		printf("%3d: ", i);
		printf("%10.2e ", fabs((vec_x[i] - vec_x_iter[i]) / vec_x[i]));
		printf("\n");
	}
	
	// free
	free(mat_a);
	free(vec_x);
	free(vec_x_iter);
	free(vec_b);

	return EXIT_SUCCESS;
}