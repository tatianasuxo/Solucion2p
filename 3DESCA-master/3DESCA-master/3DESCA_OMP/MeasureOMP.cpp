#include "TDES.hpp"
#include "Timer.hpp"
#include "MeasureOMP.hpp"
#include "FileUtilities.hpp"

#include <vector>
#include <iostream>
#include <omp.h>

namespace OMP {

	double measureEncode(TDESCA::chunk64 key1, TDESCA::chunk64 key2, TDESCA::chunk64 key3, const std::string& inPath, const std::string& outPath, unsigned int threadsNumber)
	{
		std::vector<TDESCA::chunk64> dataIn = readFileIntoChunks(inPath);

		std::vector<TDESCA::chunk64> dataOut(dataIn.size());
		TDESCA::TDES cipher;
		Timer timer;

		timer.start();
		omp_set_num_threads(threadsNumber);
		#pragma omp parallel for shared(dataOut, dataIn)
		for (int i = 0; i < static_cast<int>(dataIn.size()); i++)
			dataOut[i] = cipher.Encode(key1, key2, key3, dataIn[i]).val;

		double resultNs = timer.stopNs();
		saveChunksIntoFile(outPath, dataOut);

		return resultNs;
	}

	double measureDecode(TDESCA::chunk64 key1, TDESCA::chunk64 key2, TDESCA::chunk64 key3, const std::string& inPath, const std::string& outPath, unsigned int threadsNumber)
	{
		std::vector<TDESCA::chunk64> dataIn = readFileIntoChunks(inPath);

		std::vector<TDESCA::chunk64> dataOut(dataIn.size());
		TDESCA::TDES cipher;
		Timer timer;

		timer.start();
		omp_set_num_threads(threadsNumber);
		#pragma omp parallel for shared(dataOut, dataIn)
		for (int i = 0; i < static_cast<int>(dataIn.size()); i++)
			dataOut[i] = cipher.Decode(key1, key2, key3, dataIn[i]).val;
	
		double resultNs = timer.stopNs();
		saveChunksIntoFile(outPath, dataOut);

		return resultNs;
	}

	std::pair<double, double> measure(TDESCA::chunk64 key1, TDESCA::chunk64 key2, TDESCA::chunk64 key3, std::string inPath, unsigned int threadsNumber, unsigned int repeatTimes)
	{
		std::string encPath = inPath + ".enc";
		std::string decPath = encPath + ".dec";
		std::pair<double, double> resultNs{ 0.0, 0.0 };

		for (unsigned int i = 0; i < repeatTimes; i++)
		{
			resultNs.first += measureEncode(key1, key2, key3, inPath, encPath, threadsNumber);
			resultNs.second += measureDecode(key1, key2, key3, encPath, decPath, threadsNumber);
		}

		return resultNs;
	}

} // namespace OMP