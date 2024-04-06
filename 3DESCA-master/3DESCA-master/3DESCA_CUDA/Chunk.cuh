#pragma once

#include <array>

namespace TDESCA {

union chunk64
{
    uint8_t arr[8];
    uint64_t val;
};

union chunk48
{
    uint8_t arr[8];
    uint64_t val;
};

union chunk32
{
    uint8_t arr[4];
    uint32_t val;
};

} // namespace TDESCA
