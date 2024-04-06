#include <Chunk.hpp>

namespace CUDA {

void CudaEncode(TDESCA::chunk64 key1, TDESCA::chunk64 key2,
                TDESCA::chunk64 key3, TDESCA::chunk64* dataIn,
                unsigned int chunkCount, unsigned int threadsNum,
                TDESCA::chunk64* dataOut, double* timeOut);

void CudaDecode(TDESCA::chunk64 key1, TDESCA::chunk64 key2,
                TDESCA::chunk64 key3, TDESCA::chunk64* dataIn,
                unsigned int chunkCount, unsigned int threadsNum,
                TDESCA::chunk64* dataOut, double* timeOut);

} // namespace CUDA