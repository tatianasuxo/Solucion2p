#pragma once

#include <cstdint>
#include "Chunk.cuh"


namespace TDESCA {

class DES
{
    __device__ chunk64 InitPerm(chunk64 msg);
    __device__ chunk64 FinalPerm(chunk64 ciph);
    __device__ chunk64 Expansion(chunk32 data);
    __device__ uint8_t Substitution(uint8_t sBoxNo, uint8_t inputVal);
    __device__ chunk32 SBoxPermutation(chunk32 data);
    __device__ chunk64 PermutedChoice1(chunk64 key); // in fact outputs 56-bit key
    __device__ chunk64 PermutedChoice2(chunk64 key); // in fact outputs 48-bit key
    __device__ chunk32 Feistel(chunk32 data, chunk64 subKey);
    __device__ uint32_t RotateHalfkeyLeft(chunk32 halfKey, uint8_t n);
    __device__ chunk64 Process(chunk64 key, chunk64 data, bool isDecode);

public:
    __device__ DES() {};
    __device__ ~DES() {};

    __device__ chunk64 Encode(chunk64 key, chunk64 data);
    __device__ chunk64 Decode(chunk64 key, chunk64 data);
};

} // namespace TDESCA
