#pragma once

#include "Bit.hpp"

namespace MPI {
std::pair<double, double> measure(TDESCA::chunk64 key1, TDESCA::chunk64 key2, TDESCA::chunk64 key3, std::string inPath, unsigned int repeatTimes = 1);
} // namespace MPI