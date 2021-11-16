#ifndef PAQ8PX_NORMALMODEL_HPP
#define PAQ8PX_NORMALMODEL_HPP

#include "../ContextMap2.hpp"
#include "../LargeStationaryMap.hpp"

class NormalModel {
private:
    static constexpr int nCM = ContextMap2::C; // 4
    static constexpr int nLM = 5;
    static constexpr int nSM = 4;
    Shared * const shared;
    uint64_t width{}; // shared->width* shared->fileFormat;
    uint64_t ctx1{};
    uint64_t ctx2{};
    uint64_t ctx3{};
    uint64_t ctx1N{};
    uint64_t ctx2N{};
    uint64_t ctx3N{};
    uint64_t bitStream{};
    uint32_t W{}, WW{}, N{}, NN{};
    uint8_t col{};

    union {
      uint32_t  x;
      float  f;
    } temp{};

    float bintofloat(uint32_t x) {
      temp.x = x;
      return temp.f;
    }

    uint32_t floattobin(float f) {
      temp.f = f;
      return temp.x;
    }

public:
    static constexpr int MIXERINPUTS = 
      nCM * ContextMap2::MIXERINPUTS +
      nLM * LargeStationaryMap::MIXERINPUTS +
      + nSM; // 22
    static constexpr int MIXERCONTEXTS =
      4 * 8 + //256
      8 * 4 * 255 + //8160
      2 * 256 * 4 + //4096
      3 * 3 * 3 * 3 + //81
      3 * 3 * 3 * 3 * 3 //243
    ; // 10564
    static constexpr int MIXERCONTEXTSETS = 5;
    NormalModel(Shared* const sh, const uint64_t cmSize);

    ContextMap2 cm;
    StateMap smOrder0;
    StateMap smOrder1;
    LargeStationaryMap largeMap;

    void mix(Mixer &m);
};

#endif //PAQ8PX_NORMALMODEL_HPP
