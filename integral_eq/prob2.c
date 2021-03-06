/*************************************************/
/* LAPACK/BLAS Tutorial                          */
/* Hammerstein Integral equation                 */
/* Last Update: 2016-12-02 (Fri) T.Kouya         */
/*************************************************/
//
// Problem 8.2: x(s) = s^2+ sin(s) * int[-1, 1] K(s, t) (x(t))^2 dt
//              where K(s, t) = exp(-2t)
//
double get_bij(int i, int j, double *abscissa, double *weight)
{
	double ret;

	// abscissa[i] = s_i
	// abscissa[j] = t_j
	ret = weight[j] * exp(-2.0 * abscissa[j]) * sin(abscissa[i]);

	return ret;
}

void get_bmatrix(double *B, int row_dim, int col_dim, double *abscissa, double *weight)
{
	int i, j;

#ifdef _OPENMP
	#pragma omp parallel for private(j)
#endif // _OPENMP
	for(i = 0; i < row_dim; i++)
		for(j = 0; j < col_dim; j++)
			B[i * col_dim + j] = get_bij(i, j, abscissa, weight);

}

double *dmat_b;
double *dvec_abscissa, *dvec_weight, *dvec_ssqr, *dvec_xsqr;

void init_derivative_free_iteration_dvector(int dim)
{
	dvec_abscissa = (double *)calloc(dim, sizeof(double));
	dvec_weight = (double *)calloc(dim, sizeof(double));
	dvec_xsqr = (double *)calloc(dim, sizeof(double));
	dvec_ssqr = (double *)calloc(dim, sizeof(double));

	gauss_integral_eig_d(dvec_abscissa, dvec_weight, dim, GAUSS_LEGENDRE);
	dshifted_gauss_legendre(-1.0, 1.0, dvec_abscissa, dvec_weight, dim);

	dmat_b = (double *)calloc(dim * dim, sizeof(double));

	get_bmatrix(dmat_b, dim, dim, dvec_abscissa, dvec_weight);

}

void free_derivative_free_iteration_dvector(void)
{
	free(dvec_abscissa);
	free(dvec_weight);
	free(dvec_xsqr);
	free(dvec_ssqr);

	free(dmat_b);
}

// ret = x^2
void sqr_dvector(double *ret, double *x, int dim)
{
	int i;

#ifdef _OPENMP
	#pragma omp parallel for
#endif // _OPENMP
	for(i = 0; i < dim; i++)
		ret[i] = x[i] * x[i];
}

// ret = exp(s) - s * sin(s)
void ssqr_dvector(double *ret, double *s, int dim)
{
	int i;

#ifdef _OPENMP
	#pragma omp parallel for
#endif // _OPENMP
	for(i = 0; i < dim; i++)
		ret[i] = s[i] * s[i];
}


// double vfunc_index(int, DVector)
double vf_index(int index, double *x, int dim)
{
	double ret, tmp;
	int j;

	// B * x_sqr
	tmp = 0.0;
#ifdef _OPENMP
	#pragma omp parallel for reduction(+:tmp)
#endif // _OPENMP
	for(j = 0; j < dim; j++)
		tmp += dmat_b[index * dim + j] * (x[j] * x[j]);

	ret = x[index] - dvec_abscissa[index] * dvec_abscissa[index] - tmp;

	return ret;
}


// void vfunc(DVector, DVector)
void vf(double *ret_vec, double *x, int dim)
{
	double ret;
	int i;
	double *tmp_vec;

	tmp_vec = (double *)calloc(dim, sizeof(double));

	//  ret_vec := B * x_sqr
	sqr_dvector(dvec_xsqr, x, dim);
	cblas_dgemv(CblasRowMajor, CblasNoTrans, dim, dim, 1.0, dmat_b, dim, dvec_xsqr, 1, 0.0, ret_vec, 1);

	// ret_vec := s * s + B * x_sqr
	ssqr_dvector(tmp_vec, dvec_abscissa, dim);
	cblas_daxpy(dim, 1.0, tmp_vec, 1, ret_vec, 1);

	// ret_vec := -ret_vec
	cblas_dscal(dim, -1.0, ret_vec, 1);

	// ret_vec := x + ret_vec
	cblas_daxpy(dim, 1.0, x, 1, ret_vec, 1);

	free(tmp_vec);
}
