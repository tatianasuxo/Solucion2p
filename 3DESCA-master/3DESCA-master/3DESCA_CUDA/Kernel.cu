#include "TDES.cuh"
#include "Chunk.cuh"
#include <Timer.hpp>

#include <cuda.h>

#include <iostream>

namespace CUDA {

__global__ void kernelEncode(TDESCA::chunk64* keys, TDESCA::chunk64* dataIn,
                             unsigned int threadCount, TDESCA::chunk64* dataOut)
{
    int ind = threadIdx.x + blockIdx.x * threadCount;
    TDESCA::TDES cipher;
    dataOut[ind] = cipher.Encode(keys[0], keys[1], keys[2], dataIn[ind]);
}

__global__ void kernelDecode(TDESCA::chunk64* keys, TDESCA::chunk64* dataIn,
                             unsigned int threadCount, TDESCA::chunk64* dataOut)
{
    int ind = threadIdx.x + blockIdx.x * threadCount;
    TDESCA::TDES cipher;
    dataOut[ind] = cipher.Decode(keys[0], keys[1], keys[2], dataIn[ind]);
}

void CudaEncode(TDESCA::chunk64 key1, TDESCA::chunk64 key2,
                TDESCA::chunk64 key3, TDESCA::chunk64* dataIn,
                unsigned int chunkCount, unsigned int threadsNum,
                TDESCA::chunk64* dataOut, double* timeOut)
{
    Timer timer;

    TDESCA::chunk64* cudaDataIn;
    TDESCA::chunk64* cudaDataOut;
    TDESCA::chunk64* cudaKeys;

    cudaMalloc(&cudaDataIn, chunkCount * sizeof(TDESCA::chunk64));
    cudaMalloc(&cudaKeys, 3 * sizeof(TDESCA::chunk64));
    cudaMalloc(&cudaDataOut, chunkCount * sizeof(TDESCA::chunk64));

    TDESCA::chunk64 keys[] = { key1, key2, key3 };

    cudaMemcpy(cudaDataIn, dataIn, chunkCount * sizeof(TDESCA::chunk64), cudaMemcpyHostToDevice);
    cudaMemcpy(cudaKeys, keys, 3 * sizeof(TDESCA::chunk64), cudaMemcpyHostToDevice);

    unsigned int blockCount = chunkCount / threadsNum;

    timer.start();
    kernelEncode<<<blockCount, threadsNum>>>(cudaKeys, cudaDataIn, threadsNum, cudaDataOut);
    cudaDeviceSynchronize();
    *timeOut = timer.stopNs();

    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        std::cout << "CUDA Kernel failed to launch with error code " << err << std::endl;
        *timeOut = -1.0;
    }
    else
        err = cudaMemcpy(dataOut, cudaDataOut, chunkCount * sizeof(TDESCA::chunk64), cudaMemcpyDeviceToHost);

    cudaFree(cudaDataIn);
    cudaFree(cudaKeys);
    cudaFree(cudaDataOut);
}

void CudaDecode(TDESCA::chunk64 key1, TDESCA::chunk64 key2,
                TDESCA::chunk64 key3, TDESCA::chunk64* dataIn,
                unsigned int chunkCount, unsigned int threadsNum,
                TDESCA::chunk64* dataOut, double* timeOut)
{
    Timer timer;

    TDESCA::chunk64* cudaDataIn;
    TDESCA::chunk64* cudaDataOut;
    TDESCA::chunk64* cudaKeys;
    cudaMalloc(&cudaDataIn, chunkCount * sizeof(TDESCA::chunk64));
    cudaMemcpy(cudaDataIn, dataIn, chunkCount * sizeof(TDESCA::chunk64), cudaMemcpyHostToDevice);

    cudaMalloc(&cudaKeys, 3 * sizeof(TDESCA::chunk64));
    TDESCA::chunk64 keys[] = { key1, key2, key3 };
    cudaMemcpy(cudaKeys, keys, 3 * sizeof(TDESCA::chunk64), cudaMemcpyHostToDevice);

    cudaMalloc(&cudaDataOut, chunkCount * sizeof(TDESCA::chunk64));

    unsigned int blockCount = chunkCount / threadsNum;

    timer.start();
    kernelDecode<<<blockCount, threadsNum>>>(cudaKeys, cudaDataIn, threadsNum, cudaDataOut);
    cudaDeviceSynchronize();
    *timeOut = timer.stopNs();

    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess)
    {
        std::cout << "CUDA Kernel failed to launch with error code " << err << std::endl;
        *timeOut = -1.0;
    }
    else
        cudaMemcpy(dataOut, cudaDataOut, chunkCount * sizeof(TDESCA::chunk64), cudaMemcpyDeviceToHost);

    cudaFree(cudaDataIn);
    cudaFree(cudaDataOut);
    cudaFree(cudaKeys);
}

} // namespace CUDA