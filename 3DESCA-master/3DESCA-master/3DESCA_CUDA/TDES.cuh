#pragma once

#include "DES.cuh"


namespace TDESCA {

class TDES
{
    DES cipherMachine;

public:
    __device__ TDES() {};
    __device__ ~TDES() {};

    __device__ chunk64 Encode(chunk64 key1, chunk64 key2, chunk64 key3, chunk64 data);
    __device__ chunk64 Decode(chunk64 key1, chunk64 key2, chunk64 key3, chunk64 data);
};

} // namespace TDESCA