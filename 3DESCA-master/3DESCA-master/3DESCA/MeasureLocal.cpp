#include "TDES.hpp"
#include "Timer.hpp"
#include "MeasureLocal.hpp"
#include "FileUtilities.hpp"

#include <vector>
#include <iostream>

namespace Local {

double measureEncode(TDESCA::chunk64 key1, TDESCA::chunk64 key2, TDESCA::chunk64 key3, const std::string& inPath, const std::string& outPath)
{
    std::vector<TDESCA::chunk64> dataIn = readFileIntoChunks(inPath);

    std::vector<TDESCA::chunk64> dataOut;
    dataOut.reserve(dataIn.size());
    TDESCA::TDES cipher;
    Timer timer;

    timer.start();
    for (const auto& i : dataIn)
        dataOut.push_back(cipher.Encode(key1, key2, key3, i).val);
    double resultNs = timer.stopNs();
    saveChunksIntoFile(outPath, dataOut);

    return resultNs;
}

double measureDecode(TDESCA::chunk64 key1, TDESCA::chunk64 key2, TDESCA::chunk64 key3, const std::string& inPath, const std::string& outPath)
{
    std::vector<TDESCA::chunk64> dataIn = readFileIntoChunks(inPath);

    std::vector<TDESCA::chunk64> dataOut;
    dataOut.reserve(dataIn.size());
    TDESCA::TDES cipher;
    Timer timer;

    timer.start();
    for (const auto& i : dataIn)
        dataOut.push_back(cipher.Decode(key1, key2, key3, i).val);
    double resultNs = timer.stopNs();
    saveChunksIntoFile(outPath, dataOut);

    return resultNs;
}

std::pair<double, double> measure(TDESCA::chunk64 key1, TDESCA::chunk64 key2, TDESCA::chunk64 key3, std::string inPath, unsigned int repeatTimes)
{
    std::string encPath = inPath + ".enc";
    std::string decPath = encPath + ".dec";
    std::pair<double, double> resultNs{0.0, 0.0};

    for (unsigned int i = 0; i < repeatTimes; i++)
    {
        resultNs.first += measureEncode(key1, key2, key3, inPath, encPath);
        resultNs.second += measureDecode(key1, key2, key3, encPath, decPath);
    }

    return resultNs;
}

} // namespace Local