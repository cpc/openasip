/**
 * @file Swapper.hh
 *
 * Declaration of Swapper class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note reviewed 23 October 2003 by am, jn, ll, pj
 *
 * @note rating: yellow
 */

#ifndef TTA_SWAPPER_HH
#define TTA_SWAPPER_HH

#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Swaps byte order of type T data element if necessary.
 *
 * Swapping from/to big-endian byte order takes place if the host data is
 * encoded using "little-endian" byte order (least significant byte at the
 * lowest address).  Swapping from host to target or from target to host
 * byte order takes place if host and target byte orders differ.
 *
 * This class is a mere collection of member functions. Though it's
 * not prevented, there's no need to create objects of this class.
 */
class Swapper {
public:
    template <typename T>
    static void fromHostToTargetByteOrder(T source, Byte* dest);

    template <typename T>
    static void fromTargetToHostByteOrder(Byte* source, T& dest);

    template <typename T>
    static void fromBigEndianToHostByteOrder(Byte* source, T& dest);

    template <typename T>
    static void fromHostToBigEndianByteOrder(T source, Byte* dest);

    template <typename T>
    static void fromHostToLittleEndianByteOrder(T source, Byte* dest);

private:
    static bool needHostTargetSwap();
    static bool isHostBigEndian();
    static void swap(Byte* p, int size);
};
}

#include "Swapper.icc"

#endif
