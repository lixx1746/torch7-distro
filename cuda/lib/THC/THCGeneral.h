#ifndef THC_GENERAL_INC
#define THC_GENERAL_INC

#include "THGeneral.h"

#include "cuda.h"
#include "cublas.h"
#include "cuda_runtime_api.h"

#define THCudaCheck(err)  __THCudaCheck(err, __FILE__, __LINE__)

TH_API void __THCudaCheck(cudaError_t err, const char *file, const int line);

#endif
