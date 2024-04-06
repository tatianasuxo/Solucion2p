#include <fstream>
#include <windows.h>
#include "FileUtilities.hpp"


std::string ExePath()
{
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}

std::vector<TDESCA::chunk64> readFileIntoChunks(std::string filePath)
{
    std::ifstream myfile(filePath, std::ios::in | std::ios::binary);
    TDESCA::chunk64 chunk;
    std::vector<TDESCA::chunk64> dataIn;
    while (myfile)
    {
        uint32_t val[2] = {0};
        if (myfile.read((char*)val, sizeof(val)))
        {
            chunk.val = ((uint64_t)val[1]) << 32 | (uint64_t)val[0];
            dataIn.push_back(chunk);
        }
    }
    myfile.close();
    return dataIn;
}

void  saveChunksIntoFile(std::string filePath, std::vector<TDESCA::chunk64> chunks)
{
    std::ofstream fout(filePath, std::ios::out | std::ios::binary);
    fout.write(reinterpret_cast<const char*>(chunks.data()), chunks.size() * sizeof(uint64_t));
    fout.close();
}
