#pragma once
#include <limits>

template <typename T>
constexpr T Rol(T val, int n)
{
    static_assert(std::is_unsigned<T>::value, "Why would You even rotate NOT an unsigned type?");
    return (val << n) | (val >> (sizeof(T) * CHAR_BIT - n));
}

template <typename T>
constexpr T Ror(T val, int n)
{
    static_assert(std::is_unsigned<T>::value, "Why would You even rotate NOT an unsigned type?");
    return (val >> n) | (val << (sizeof(T) * CHAR_BIT - n));
}

#define GETBIT(num, n) ((num >> n) & 0x1)
#define SETBIT(num, n, val) (num = (num | ((val & 0x1) << n)));