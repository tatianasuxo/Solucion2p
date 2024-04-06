#pragma once

#include "DES.hpp"


namespace TDESCA {

class TDES
{
    DES cipherMachine;

public:
    TDES() {};
    ~TDES() {};

    chunk64 Encode(chunk64 key1, chunk64 key2, chunk64 key3, chunk64 data);
    chunk64 Decode(chunk64 key1, chunk64 key2, chunk64 key3, chunk64 data);
};

} // namespace TDESCA