#pragma once

#include <vector>
#include "Chunk.hpp"

std::string ExePath();
std::vector<TDESCA::chunk64> readFileIntoChunks(std::string filePath);
void saveChunksIntoFile(std::string filePath, std::vector<TDESCA::chunk64> chunks);