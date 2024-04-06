#pragma once
#include <limits>

template <typename T>
__device__ T Rol(T val, int n)
{
    return (val << n) | (val >> (sizeof(T) * CHAR_BIT - n));
}

template <typename T>
__device__ T Ror(T val, int n)
{
    return (val >> n) | (val << (sizeof(T) * CHAR_BIT - n));
}

#define GETBIT(num, n) ((num >> n) & 0x1)
#define SETBIT(num, n, val) (num = (num | ((val & 0x1) << n)));