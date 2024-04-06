#pragma once

#include <array>

namespace TDESCA {

union chunk64
{
    std::array<uint8_t, 8> arr;
    uint64_t val;

    // arr is inverted val:
    // val = 0x0102030405060708 will result in
    // arr = {8, 7, 6, 5, 4, 3, 2, 1};
    chunk64()
        : val(0)
    {
    }

    chunk64(uint64_t v)
        : val(v)
    {
    }
};

union chunk48
{
    std::array<uint8_t, 8> arr;
    uint64_t val;
};

union chunk32
{
    std::array<uint8_t, 4> arr;
    uint32_t val;
};

} // namespace TDESCA
