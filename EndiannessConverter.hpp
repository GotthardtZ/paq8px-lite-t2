#pragma once

#include "file/FileDisk.hpp"

void SwapBytes(FileDisk &inputFile, FileDisk& outputFile, uint64_t fileSize, int bytesToSwap /*2 or 4*/);