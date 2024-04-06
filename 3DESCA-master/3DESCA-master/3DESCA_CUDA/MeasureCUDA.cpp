#pragma warning (disable : 4267)
#pragma warning (disable : 4930)

#include "TDES.hpp"
#include "Timer.hpp"
#include "MeasureCUDA.hpp"
#include "FileUtilities.hpp"
#include "Kernel.cuh"

#include <vector>
#include <iostream>


namespace CUDA {

double measureEncode(TDESCA::chunk64 key1, TDESCA::chunk64 key2, TDESCA::chunk64 key3, const std::string& inPath,
                     const std::string& outPath, unsigned int threadsNumber)
{
    std::vector<TDESCA::chunk64> inputChunks;
    std::vector<int> dataInSizes;

    // ENCODING
    inputChunks = readFileIntoChunks(inPath);

    // input data must be a multiple of thread count per block
    // which we assume to be 256
    size_t originalSize = inputChunks.size();
    if (originalSize % threadsNumber != 0)
    {
        size_t missingSize = threadsNumber - originalSize % threadsNumber;
        inputChunks.resize(originalSize + missingSize);
    }

    std::vector<TDESCA::chunk64> outputChunks;
    outputChunks.resize(inputChunks.size());

    double resultNs;
    CudaEncode(key1, key2, key3, inputChunks.data(), inputChunks.size(), threadsNumber, outputChunks.data(), &resultNs);

    if (resultNs == -1.0)
    {
        return -1.0;
    }

    // revert output to original size
    outputChunks.resize(originalSize);
    saveChunksIntoFile(outPath, outputChunks);
    return resultNs;
}

double measureDecode(TDESCA::chunk64 key1, TDESCA::chunk64 key2, TDESCA::chunk64 key3, const std::string& inPath,
                     const std::string& outPath, unsigned int threadsNumber)
{
    std::vector<TDESCA::chunk64> inputChunks;

    // DECODING
    inputChunks = readFileIntoChunks(inPath);

    // input data must be a multiple of thread count per block
    size_t originalSize = inputChunks.size();
    if (originalSize % threadsNumber != 0)
    {
        size_t missingSize = threadsNumber - originalSize % threadsNumber;
        inputChunks.resize(originalSize + missingSize);
    }

    std::vector<TDESCA::chunk64> outputChunks;
    outputChunks.resize(inputChunks.size());

    double resultNs;
    CudaDecode(key1, key2, key3, inputChunks.data(), inputChunks.size(), threadsNumber, outputChunks.data(), &resultNs);

    if (resultNs == -1.0)
    {
        return -1.0;
    }

    outputChunks.resize(originalSize);
    saveChunksIntoFile(outPath, outputChunks);
    return resultNs;
}

std::pair<double, double> measure(TDESCA::chunk64 key1, TDESCA::chunk64 key2, TDESCA::chunk64 key3, std::string inPath,
                                  unsigned int threadsNumber, unsigned int repeatTimes)
{
    std::string encPath = inPath + ".enc";
    std::string decPath = encPath + ".dec";
    std::pair<double, double> resultNs{0.0, 0.0}, blank;
    double temp1, temp2;

    if (threadsNumber > 1024)
    {
        std::cout << "Threads number cannot be higher than 1024" << std::endl;
        return std::make_pair(-1.0, -1.0);
    }

    for (unsigned int i = 0; i < repeatTimes; i++)
    {
        temp1 = measureEncode(key1, key2, key3, inPath, encPath, threadsNumber);
        temp2 = measureDecode(key1, key2, key3, encPath, decPath, threadsNumber);

        if (temp1 == -1.0 || temp2 == -1.0)
        {
            resultNs.first = temp1;
            resultNs.second = temp2;
            return resultNs;
        }

        resultNs.first += temp1;
        resultNs.second += temp2;
    }

    return resultNs;
}

} // namespace MPI