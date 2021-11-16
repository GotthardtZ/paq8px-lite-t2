#include <cstdio>
#include <cstdlib>
#include "EndiannessConverter.hpp"

//A simple endianness converter
void SwapBytes(FileDisk &inputFile, FileDisk& outputFile, uint64_t fileSize, int bytesToSwap /*2 or 4*/)
{
  printf("\nChanging endianness - swapping %d bytes (using tempfile) --> ", bytesToSwap);

  uint8_t* buffer = (uint8_t*)malloc(fileSize);
  if (!buffer) {
    fprintf(stderr, "Out of memory\n.");
    return;
  }

  inputFile.blockRead(buffer, fileSize);
  if (bytesToSwap == 2) {
    for (uint64_t i = 0; i < fileSize; i += 2)
    {
      outputFile.putChar(buffer[i + 1]);
      outputFile.putChar(buffer[i + 0]);
    }
  }
  else if (bytesToSwap == 4) {
    for (uint64_t i = 0; i < fileSize; i += 4)
    {
      outputFile.putChar(buffer[i + 3]);
      outputFile.putChar(buffer[i + 2]);
      outputFile.putChar(buffer[i + 1]);
      outputFile.putChar(buffer[i + 0]);
    }
  }
  free(buffer);

  printf("Done.\n\n");

}