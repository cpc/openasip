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
 * @file Swapper11Test.hh
 *
 * A test suite for Swapper. Host and target machines are defined as
 * big-endian.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note reviewed 23 October 2003 by am, jn, ll, pj
 */

#ifndef TTA_SWAPPER_11_TEST_HH
#define TTA_SWAPPER_11_TEST_HH

#include <TestSuite.h>
#include "BaseType.hh"

#if WORDS_BIGENDIAN
#define TEST_HOST_BIGENDIAN 1
#else
#define TEST_HOST_BIGENDIAN 0
#endif

#undef WORDS_BIGENDIAN
#undef TARGET_WORDS_BIGENDIAN
#define WORDS_BIGENDIAN 1
#define TARGET_WORDS_BIGENDIAN 1

#include "SwapperTestCore.hh"

using namespace TPEF;

/**
 * Implements the test needed to verify correct operation of Swapper
 * when host and target machines are defined as big-endian.
 */
class Swapper11Test : public CxxTest::TestSuite {
public:
    void testFromHostToTarget();
    void testFromTargetToHost();
    void testFromHostToBigEndian();
    void testFromBigEndianToHost();
};

const Word SwapperTestCore::wordSwapped_ = 0x01020304;
const Word SwapperTestCore::word_ = 0x04030201;

const HalfWord SwapperTestCore::halfWordSwapped_ = 0xff00;
const HalfWord SwapperTestCore::halfWord_ = 0x00ff;

const Byte SwapperTestCore::wordBytesRef_[4] = {
    0x01, 0x02, 0x03, 0x04
};

const Byte SwapperTestCore::halfWordBytesRef_[2] = {
    0xff, 0x00
};

/**
 * Tests that fromHostToTargetByteOrder works correctly.
 */
inline void
Swapper11Test::testFromHostToTarget() {
    SwapperTestCore::fromHostToTarget();
}
    
/**
 * Tests that fromTargetToHostByteOrder works correctly.
 */
inline void
Swapper11Test::testFromTargetToHost() {
    SwapperTestCore::fromTargetToHost();
}

/**
 * Tests that fromHostToBigEndianByteOrder works correctly.
 */
inline void
Swapper11Test::testFromHostToBigEndian() {
    SwapperTestCore::fromHostToBigEndian();
}

/**
 * Tests that fromBigEndianToHostByteOrder works correctly.
 */
inline void
Swapper11Test::testFromBigEndianToHost() {
    SwapperTestCore::fromBigEndianToHost();
}

#endif
