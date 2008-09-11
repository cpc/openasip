/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
