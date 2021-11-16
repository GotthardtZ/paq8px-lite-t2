#include "NormalModel.hpp"

NormalModel::NormalModel(Shared* const sh, const uint64_t cmSize) :
  shared(sh),
  width(sh->width * sh->fileFormat),
  cm(sh, cmSize),
  smOrder0(sh, 255 * 4, 4),
  smOrder1(sh, 255 * 256 * 4, 32),
  largeMap(sh, nLM, 15 + sh->level) //16..27
{
  assert(isPowerOf2(cmSize));
}

void NormalModel::mix(Mixer &m) {
  INJECT_SHARED_bpos
  INJECT_SHARED_c1
  INJECT_SHARED_c4
  INJECT_SHARED_pos
  INJECT_SHARED_buf
  if( bpos == 0 ) {

    col = pos & (shared->fileFormat-1);

    uint64_t i = 0;
    if (shared->fileFormat == 2) {
      if (col == 0) {
        WW = W;
        W = c4 & 0x0000ffff;
        N = buf(1 * width) << 8 | buf(1 * width - 1);
        NN = buf(2 * width) << 8 | buf(2 * width - 1);
        ctx1 = W + WW;
        ctx2 = N + NN;
        ctx3 = W + N;
      }
      const uint32_t lastChunk0 = col == 0 ? 0 : c1;
      const uint32_t lastChunk1 = col == 0 ? W >> 8 : W;
      const uint32_t lastChunk2 = col == 0 ? WW >> 8 : WW;
      cm.set(i, hash(i, lastChunk0, col));
      i++;
      cm.set(i, hash(i, lastChunk0, lastChunk1 , col));
      i++;
      cm.set(i, hash(i, lastChunk0, lastChunk1, lastChunk2, col));
      i++;
      cm.set(i, hash(i, N, W, lastChunk0, col));
    }
    else {
      if (col == 0) {
        WW = W;
        W = c4;
        N = buf(1 * width) << 24 | buf(1 * width - 1) << 16 | buf(1 * width - 2) << 8 | buf(1 * width - 3);
        NN = buf(2 * width) << 24 | buf(2 * width - 1) << 16 | buf(2 * width - 2) << 8 | buf(2 * width - 3);
        float Wf = bintofloat(W);
        float WWf = bintofloat(WW);
        float Nf = bintofloat(N);
        float NNf = bintofloat(NN);
        ctx1 = floattobin(Wf + WWf);
        ctx2 = floattobin(Nf + NNf);
        ctx3 = floattobin(Wf + Nf);
      }
      const uint32_t lastChunk0 = c4 & (0x00ffffff >> ((3 - col) * 8));
      const uint32_t lastChunk1 = W >> ((3 - col) * 8);
      const uint32_t lastChunk2 = WW >> ((3 - col) * 8);
      cm.set(i, hash(i, lastChunk0, col));
      i++;
      cm.set(i, hash(i, lastChunk0, lastChunk1, col));
      i++;
      cm.set(i, hash(i, lastChunk0, lastChunk1, lastChunk2, col));
      i++;
      cm.set(i, hash(i, N, W, lastChunk0, col));
    }
  }
  cm.mix(m);

  INJECT_SHARED_y
  bitStream <<= 1;
  bitStream |= y;

  int bitPos = col * 8 + bpos; //0..15; 0..31
  if (bitPos == 0)
    bitStream = 1;

  int prefixLength;

  prefixLength = (shared->fileFormat * 8 - 1 - bitPos);
  prefixLength = max(prefixLength - 3, 0); //3 bits lookahead

  //note: we deliberately allow hash collisions
  largeMap.set(hash(bitStream, ctx1 >> prefixLength));
  largeMap.set(hash(bitStream, ctx2 >> prefixLength));
  largeMap.set(hash(bitStream, ctx3 >> prefixLength));

  uint64_t before1 = W >> prefixLength;
  uint64_t before2 = WW >> prefixLength;
  uint64_t above1 = N >> prefixLength;
  uint64_t above2 = NN >> prefixLength;

  largeMap.set(hash(bitStream, before1, above1));
  largeMap.set(hash(bitStream, before1, before2, above1, above2));

  largeMap.mix(m);

  INJECT_SHARED_c0
  int p1, st;
  p1 = smOrder0.p1((c0 - 1) << 2 | col);
  m.add((p1 - 2048) >> 2);
  st = stretch(p1);
  m.add(st >> 1);

  p1 = smOrder1.p1((c0 - 1) << 10 | c1 << 2 | col);
  m.add((p1 - 2048) >> 2);
  st = stretch(p1);
  m.add(st >> 1);

  uint32_t misses = shared->State.misses << ((8 - bpos) & 7); //byte-aligned
  misses = (misses & 0xffffff00) | (misses & 0xff) >> ((8 - bpos) & 7);

  uint32_t misses3 =
    ((misses & 0x1) != 0) |
    ((misses & 0xfe) != 0) << 1 |
    ((misses & 0xff00) != 0) << 2;
   
  m.set(col << 3 | bpos, 4 * 8);
  m.set(((misses3) << 2 | col) * 255 + (c0 - 1), 8 * 4 * 255);
  m.set((misses != 0) << 10 | c1 << 2 | col, 2 * 256 * 4);
  m.set(cm.confidence, 3 * 3 * 3 * 3);
  m.set(largeMap.confidence, 3 * 3 * 3 * 3 * 3);

}

