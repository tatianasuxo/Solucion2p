#pragma warning (disable : 4267)
#pragma warning (disable : 4930)

#include "TDES.hpp"
#include "Timer.hpp"
#include "MeasureMPI.hpp"
#include "FileUtilities.hpp"

#include <vector>
#include <iostream>

#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/mpi/collectives.hpp>
#include <boost/serialization/vector.hpp>

namespace boost {
namespace serialization {
template<class Archive>
void serialize(Archive& ar, TDESCA::chunk64& chunk, const unsigned int version)
{
    ar & chunk.val;
}
} // namespace boost::serialization

namespace mpi {

template<> struct is_mpi_datatype<TDESCA::chunk64> : public mpl::true_ {};
} // namespace boost::mpi
} // namespace boost

namespace MPI {

double measureEncode(TDESCA::chunk64 key1, TDESCA::chunk64 key2, TDESCA::chunk64 key3, const std::string& inPath, const std::string& outPath, boost::mpi::environment& env, boost::mpi::communicator& world)
{
    std::vector<TDESCA::chunk64> inputChunks;
    std::vector<int> dataInSizes;
    TDESCA::TDES cipher;
    std::vector<TDESCA::chunk64> dataIn;
    Timer timer;
    int dataInSize;

    // ENCODING
    if (world.rank() == 0)
    {
        dataIn = readFileIntoChunks(inPath);
        dataInSize = dataIn.size();
    }
    boost::mpi::broadcast(world, dataInSize, 0);
    const int chunksSize = (dataInSize - 1) / world.size() + 1;
    while(dataInSize > 0)
    {
        if (dataInSize >= chunksSize)
        {
            dataInSizes.push_back(chunksSize);
            dataInSize -= chunksSize;
        } else
        {
            dataInSizes.push_back(dataInSize);
            dataInSize = 0;
        }
    }
    inputChunks.resize(dataInSizes[world.rank()]);

    if (world.rank() == 0)
    {
        timer.start();
        boost::mpi::scatterv(world, dataIn, dataInSizes, inputChunks.data(), 0);
    } else
        boost::mpi::scatterv(world, inputChunks.data(), dataInSizes[world.rank()], 0);

    std::vector<uint64_t> outputChunks;
    outputChunks.reserve(inputChunks.size());

    for (const auto& i : inputChunks)
        outputChunks.push_back(cipher.Encode(key1, key2, key3, i).val);

    if (world.rank() == 0)
    {
        std::vector<TDESCA::chunk64> dataOut;
        dataOut.resize(dataIn.size());
        boost::mpi::gatherv(world, outputChunks, reinterpret_cast<uint64_t*>(dataOut.data()), dataInSizes, 0);
        double resultNs = timer.stopNs();
        saveChunksIntoFile(outPath, dataOut);
        return resultNs;
    }

    boost::mpi::gatherv(world, outputChunks, 0);

    return 0;
}

double measureDecode(TDESCA::chunk64 key1, TDESCA::chunk64 key2, TDESCA::chunk64 key3, const std::string& inPath, const std::string& outPath, boost::mpi::environment& env, boost::mpi::communicator& world)
{
    std::vector<TDESCA::chunk64> inputChunks;
    std::vector<int> dataInSizes;
    TDESCA::TDES cipher;
    std::vector<TDESCA::chunk64> dataIn;
    Timer timer;
    int dataInSize;

    // DECODING
    if (world.rank() == 0)
    {
        dataIn = readFileIntoChunks(inPath);
        dataInSize = dataIn.size();
    }
    boost::mpi::broadcast(world, dataInSize, 0);
    const int chunksSize = (dataInSize - 1) / world.size() + 1;
    while (dataInSize > 0)
    {
        if (dataInSize >= chunksSize)
        {
            dataInSizes.push_back(chunksSize);
            dataInSize -= chunksSize;
        }
        else
        {
            dataInSizes.push_back(dataInSize);
            dataInSize = 0;
        }
    }
    inputChunks.resize(dataInSizes[world.rank()]);

    if (world.rank() == 0)
    {
        timer.start();
        boost::mpi::scatterv(world, dataIn, dataInSizes, inputChunks.data(), 0);
    }
    else
        boost::mpi::scatterv(world, inputChunks.data(), dataInSizes[world.rank()], 0);

    std::vector<uint64_t> outputChunks;
    outputChunks.reserve(inputChunks.size());

    for (const auto& i : inputChunks)
        outputChunks.push_back(cipher.Decode(key1, key2, key3, i).val);

    if (world.rank() == 0)
    {
        std::vector<TDESCA::chunk64> dataOut;
        dataOut.resize(dataIn.size());
        boost::mpi::gatherv(world, outputChunks, reinterpret_cast<uint64_t*>(dataOut.data()), dataInSizes, 0);
        double resultNs = timer.stopNs();
        saveChunksIntoFile(outPath, dataOut);
        return resultNs;
    }

    boost::mpi::gatherv(world, outputChunks, 0);

    return 0;
}

std::pair<double, double> measure(TDESCA::chunk64 key1, TDESCA::chunk64 key2, TDESCA::chunk64 key3, std::string inPath, unsigned int repeatTimes)
{
    boost::mpi::environment env;
    boost::mpi::communicator world;
	std::string encPath = inPath + ".enc";
	std::string decPath = encPath + ".dec";
    std::pair<double, double> resultNs{0.0, 0.0}, blank;
    double temp1, temp2;

    for (unsigned int i = 0; i < repeatTimes; i++)
    {
        temp1 = measureEncode(key1, key2, key3, inPath, encPath, env, world);
        temp2 = measureDecode(key1, key2, key3, encPath, decPath, env, world);
        if (world.rank() == 0)
        {
            resultNs.first += temp1;
            resultNs.second += temp2;
        }
    }

    if (world.rank() == 0)
        return resultNs;
    return blank;
}

} // namespace MPI