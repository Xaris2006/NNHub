#pragma once

#include "armadillo"

#include <cublas_v2.h>   // cuBLAS v2 API
#include <cuda_runtime.h> // CUDA memory management
#include <stdio.h>       // For error reporting

namespace NN
{
	using RealType = float;

	using Mat = arma::Mat<RealType>;
	using Vec = arma::Col<RealType>;
	using ColVec = arma::fcolvec;
	using RowVec = arma::frowvec;
	using Cube = arma::fcube;
	
	
	namespace CuBlas
	{
        // Static cuBLAS handle for reuse across operations
        inline cublasHandle_t handle;
        inline int initialized = 0;

        // Initialize cuBLAS handle (call once or on first use)
        inline void init() 
        {
            if (initialized == 0)
            {
                int deviceCount;
                cudaError_t err = cudaGetDeviceCount(&deviceCount);
                if (err != cudaSuccess)
                {
                    printf("CUDA error: %s\n", cudaGetErrorString(err));
                    initialized = -1;
                    return;
                }
                printf("Found %d CUDA devices\n", deviceCount);
                if (deviceCount == 0)
                {
                    printf("No NVIDIA GPU detected\n");
                    initialized = -1;
                    return;
                }
                for (int i = 0; i < deviceCount; i++) 
                {
                    cudaDeviceProp prop;
                    cudaGetDeviceProperties(&prop, i);
                    printf("Device %d: %s\n", i, prop.name);
                }

                cublasStatus_t status = cublasCreate(&handle);
                if (status != CUBLAS_STATUS_SUCCESS)
                {
                    printf("cuBLAS initialization failed\n");
                    initialized = -1;
                    throw std::runtime_error("cuBLAS init failed");
                }
                initialized = 1;
            }
        }

        // Cleanup function (optional, call at program exit if needed)
        inline void cleanup()
        {
            if (initialized == 1)
            {
                cublasDestroy(handle);
                initialized = 0;
            }
        }

        // Matrix * Matrix (RealType version for consistency)
        inline Mat operator*(const Mat& A, const Mat& B) 
        {
            init();

            if (initialized != 1)
                return arma::operator*(A, B);

            if (A.n_cols != B.n_rows) 
            {
                throw std::invalid_argument("Matrix dimensions mismatch for multiplication");
            }

            int m = (int)A.n_rows;  // Rows of A and C
            int k = (int)A.n_cols;  // Columns of A, rows of B
            int n = (int)B.n_cols;  // Columns of B and C

            RealType* d_A, * d_B, * d_C;
            cudaMalloc(&d_A, m * k * sizeof(RealType));
            cudaMalloc(&d_B, k * n * sizeof(RealType));
            cudaMalloc(&d_C, m * n * sizeof(RealType));

            cudaMemcpy(d_A, A.memptr(), m * k * sizeof(RealType), cudaMemcpyHostToDevice);
            cudaMemcpy(d_B, B.memptr(), k * n * sizeof(RealType), cudaMemcpyHostToDevice);

            RealType alpha = 1.0f, beta = 0.0f;
            cublasStatus_t status = cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, m, n, k, &alpha, d_A, m, d_B, k, &beta, d_C, m);
            if (status != CUBLAS_STATUS_SUCCESS) 
            {
                printf("cuBLAS Sgemm failed\n");
                cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);
                throw std::runtime_error("cuBLAS Sgemm failed");
            }

            Mat C(m, n);
            cudaMemcpy(C.memptr(), d_C, m * n * sizeof(RealType), cudaMemcpyDeviceToHost);

            cudaFree(d_A); cudaFree(d_B); cudaFree(d_C);
            return C;
        }

        // Vector * Matrix (fvec * fmat -> fvec)
        inline Vec operator*(const Vec& v, const Mat& A) 
        {
            init();

            if (initialized != 1)
                return arma::operator*(v, A);

            if (v.n_elem != A.n_rows) 
            {
                throw std::invalid_argument("Vector and matrix dimensions mismatch for multiplication");
            }

            int m = (int)A.n_rows;  // Rows of A (length of v)
            int n = (int)A.n_cols;  // Columns of A (length of result)

            RealType* d_v, * d_A, * d_y;
            cudaMalloc(&d_v, m * sizeof(RealType));
            cudaMalloc(&d_A, m * n * sizeof(RealType));
            cudaMalloc(&d_y, n * sizeof(RealType));

            cudaMemcpy(d_v, v.memptr(), m * sizeof(RealType), cudaMemcpyHostToDevice);
            cudaMemcpy(d_A, A.memptr(), m * n * sizeof(RealType), cudaMemcpyHostToDevice);

            RealType alpha = 1.0f, beta = 0.0f;
            // y = alpha * A * v + beta * y (row vector * matrix)
            cublasStatus_t status = cublasSgemv(handle, CUBLAS_OP_T, m, n, &alpha, d_A, m, d_v, 1, &beta, d_y, 1);
            if (status != CUBLAS_STATUS_SUCCESS)
            {
                printf("cuBLAS Sgemv (vec * mat) failed\n");
                cudaFree(d_v); cudaFree(d_A); cudaFree(d_y);
                throw std::runtime_error("cuBLAS Sgemv failed");
            }

            Vec y(n);
            cudaMemcpy(y.memptr(), d_y, n * sizeof(RealType), cudaMemcpyDeviceToHost);

            cudaFree(d_v); cudaFree(d_A); cudaFree(d_y);
            return y;
        }

        // Matrix * Vector (fmat * fvec -> fvec)
        inline Vec operator*(const Mat& A, const Vec& v)
        {
            init();

            if (initialized != 1)
                return arma::operator*(A, v);

            if (A.n_cols != v.n_elem)
            {
                throw std::invalid_argument("Matrix and vector dimensions mismatch for multiplication");
            }

            int m = (int)A.n_rows;  // Rows of A (length of result)
            int n = (int)A.n_cols;  // Columns of A (length of v)

            RealType* d_A, * d_v, * d_y;
            cudaMalloc(&d_A, m * n * sizeof(RealType));
            cudaMalloc(&d_v, n * sizeof(RealType));
            cudaMalloc(&d_y, m * sizeof(RealType));

            cudaMemcpy(d_A, A.memptr(), m * n * sizeof(RealType), cudaMemcpyHostToDevice);
            cudaMemcpy(d_v, v.memptr(), n * sizeof(RealType), cudaMemcpyHostToDevice);

            RealType alpha = 1.0f, beta = 0.0f;
            // y = alpha * A * v + beta * y (matrix * column vector)
            cublasStatus_t status = cublasSgemv(handle, CUBLAS_OP_N, m, n, &alpha, d_A, m, d_v, 1, &beta, d_y, 1);
            if (status != CUBLAS_STATUS_SUCCESS) 
            {
                printf("cuBLAS Sgemv (mat * vec) failed\n");
                cudaFree(d_A); cudaFree(d_v); cudaFree(d_y);
                throw std::runtime_error("cuBLAS Sgemv failed");
            }

            Vec y(m);
            cudaMemcpy(y.memptr(), d_y, m * sizeof(RealType), cudaMemcpyDeviceToHost);

            cudaFree(d_A); cudaFree(d_v); cudaFree(d_y);
            return y;
        }

	}
}