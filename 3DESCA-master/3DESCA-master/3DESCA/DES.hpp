#pragma once

#include <cstdint>
#include "Chunk.hpp"


namespace TDESCA {

class DES
{
    chunk64 InitPerm(chunk64 msg);
    chunk64 FinalPerm(chunk64 ciph);
    chunk64 Expansion(chunk32 data);
    uint8_t Substitution(uint8_t sBoxNo, uint8_t inputVal);
    chunk32 SBoxPermutation(chunk32 data);
    chunk64 PermutedChoice1(chunk64 key); // in fact outputs 56-bit key
    chunk64 PermutedChoice2(chunk64 key); // in fact outputs 48-bit key
    chunk32 Feistel(chunk32 data, chunk64 subKey);
    uint32_t RotateHalfkeyLeft(chunk32 halfKey, uint8_t n);
    chunk64 Process(chunk64 key, chunk64 data, bool isDecode);

public:
    DES() {};
    ~DES() {};

    chunk64 Encode(chunk64 key, chunk64 data);
    chunk64 Decode(chunk64 key, chunk64 data);
};

} // namespace TDESCA
