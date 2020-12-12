#ifndef PAQ8PX_LARGESTATIONARYMAP_HPP
#define PAQ8PX_LARGESTATIONARYMAP_HPP

#include "IPredictor.hpp"
#include "Bucket16.hpp"
#include "Hash.hpp"
#include "Mixer.hpp"
#include "Stretch.hpp"
#include "UpdateBroadcaster.hpp"

/**
 * Map for modelling contexts of (nearly-)stationary data.
 * The context is a hash. For each bit modelled, the exact counts of 0s and 1s are stored.
 *
 */
class LargeStationaryMap : IPredictor {
public:
    static constexpr int MIXERINPUTS = 3;

private:
    const Shared * const shared;
    Array<Bucket16> data;
    const uint32_t maskBits;
    int scale, rate;
    uint64_t context;
    uint16_t *cp;

public:
    /**
     * Construct using 2^hashMaskBits * sizeof(Bucket16) bytes of memory for storing a maximum of 2^hashMaskBits * ElementsInBucket
     * That is:
      *  hashMaskBits |       memory          |   maximum number of contexts
      *       10      |  2^10 * 42 =   43 KB  |         7 K  (~12 bits)
      *       11      |  2^11 * 42 =   86 KB  |        14 K  (~13 bits)
      *       12      |  2^12 * 42 =  172 KB  |        28 K  (~14 bits)
      *       13      |  2^13 * 42 =  344 KB  |        57 K  (~15 bits)
      *       14      |  2^14 * 42 =  688 KB  |       114 K  (~16 bits)
      *       15      |  2^15 * 42 =  1.3 MB  |       229 K  (~17 bits)
      *       16      |  2^16 * 42 =  2.7 MB  |       458 K  (~18 bits)
      *       17      |  2^17 * 42 = 11.0 MB  |       1.8 M  (~19 bits)
      *       18      |  2^18 * 42 = 22.0 MB  |       3.6 M  (~20 bits)
      *       19      |  2^19 * 42 = 44.0 MB  |       7.3 M  (~21 bits)
      *       20      |  2^20 * 42 = 88.0 MB  |      14.6 M  (~22 bits)
      *      ...              ...                         ...
      * 
     * @param bitsOfContext How many bits to use for each context. Higher bits are discarded.
     * @param inputBits How many bits [1..8] of input are to be modelled for each context. New contexts must be set at those intervals.
     * @param scale
     * @param rate
     */
    LargeStationaryMap(const Shared* const sh, const int hashMaskBits, const int scale, const int rate);

    /**
     * ctx must be a hash
     * @param ctx
     */
    void set(uint64_t ctx);
    void setscale(int scale);
    void reset();
    void update() override;
    void mix(Mixer &m);
};

#endif //PAQ8PX_LARGESTATIONARYMAP_HPP