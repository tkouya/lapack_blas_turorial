//
// mycuda.c ... CUDApîb[`
// Copyright (c) 2015 T.Kouya
//

#include "mycuda.h"

// GPUãÉsñi[ÌæðmÛ
void *mycuda_calloc(int num_elements, size_t size_element)
{
	cudaError_t cuda_error;
	void *ret = NULL;

	cuda_error = cudaMalloc((void **)&ret, num_elements * size_element);

	if(cuda_error != cudaSuccess)
	{
		printf("device memory allocation failed!(num_elements = %d, size = %d)\n", num_elements, (int)size_element);
		return NULL;
	}

	return ret;
}

// GPUãÌÌæððú
void mycuda_free(void *mem)
{
	cudaFree(mem);
}

