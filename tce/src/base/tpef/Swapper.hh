/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file Swapper.hh
 *
 * Declaration of Swapper class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
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
