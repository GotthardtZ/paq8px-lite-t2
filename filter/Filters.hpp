#ifndef PAQ8PX_FILTERS_HPP
#define PAQ8PX_FILTERS_HPP

#include "../Array.hpp"
#include "../Encoder.hpp"
#include "../file/File.hpp"
#include "../file/FileDisk.hpp"
#include "../Utils.hpp"
#include <cctype>
#include <cstdint>
#include <cstring>
#include "../EndiannessConverter.hpp"



//////////////////// Compress, Decompress ////////////////////////////


static void compressfile(const Shared* const shared, const char *filename, uint64_t fileSize, Encoder &en, bool verbose) {

  uint64_t start = en.size();

  FileDisk tmp;
  tmp.createTmp();

  FileDisk in;
  in.open(filename, true);
  SwapBytes(in, tmp, fileSize, shared->fileFormat);
  in.close();
  tmp.setpos(0);

  float p1 = 0.0f;
  float p2 = 1.0f;
  const float pscale = fileSize != 0 ? (p2 - p1) / fileSize : 0;
  p2 = p1 + pscale * fileSize;
  en.setStatusRange(p1, p2);

  fprintf(stderr, "Compressing... ");
  for (uint64_t j = 0; j < fileSize; ++j) {
    if ((j & 0xfffff) == 0) {
      en.printStatus(j, fileSize);
    }
    en.compressByte(&en.predictorMain, tmp.getchar());
  }
  fprintf(stderr, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");

  p1 = p2;

  tmp.close();
}

// Decompress or compare a file
static void decompressFile(const Shared *const shared, const char *filename, Encoder &en, uint64_t fileSize) {

  printf("Extracting");

  printf(" %s %" PRIu64 " bytes -> ", filename, fileSize);

  FileDisk tmp;
  tmp.createTmp();

  // Decompress
  for (uint64_t j = 0; j < fileSize; ++j) {
    if ((j & 0xfffff) == 0u) {
      en.printStatus();
    }
    tmp.putChar(en.decompressByte(&en.predictorMain));
  }

  FileDisk out;

  tmp.setpos(0);
  out.create(filename);
  SwapBytes(tmp, out, fileSize, shared->fileFormat);
  tmp.close();
  out.close();

  printf("done   \n");
}

#endif //PAQ8PX_FILTERS_HPP
