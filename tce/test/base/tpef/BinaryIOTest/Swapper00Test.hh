/**
 * @file Swapper00Test.hh
 *
 * A test suite for Swapper. Host and target machines are defined as
 * little-endian.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note reviewed 23 October 2003 by am, jn, ll, pj
 */

#ifndef TTA_SWAPPER_00_TEST_HH
#define TTA_SWAPPER_00_TEST_HH

#include <TestSuite.h>
#include "BaseType.hh"

#if WORDS_BIGENDIAN
#define TEST_HOST_BIGENDIAN 1
#else
#define TEST_HOST_BIGENDIAN 0
#endif

#undef WORDS_BIGENDIAN
#undef TARGET_WORDS_BIGENDIAN
#define WORDS_BIGENDIAN 0
#define TARGET_WORDS_BIGENDIAN 0

#include "SwapperTestCore.hh"

using namespace TPEF;

/**
 * Implements the test needed to verify correct operation of Swapper
 * when host and target machines are defined as little-endian.
 */
class Swapper00Test : public CxxTest::TestSuite {
public:
    void testFromHostToTarget();
    void testFromTargetToHost();
    void testFromHostToBigEndian();
    void testFromBigEndianToHost();
};

const Word SwapperTestCore::word_ = 0x01020304;
const Word SwapperTestCore::wordSwapped_ = 0x04030201;

const HalfWord SwapperTestCore::halfWord_ = 0xff00;
const HalfWord SwapperTestCore::halfWordSwapped_ = 0x00ff;

const Byte SwapperTestCore::wordBytesRef_[4] = {
    0x04, 0x03, 0x02, 0x01
};

const Byte SwapperTestCore::halfWordBytesRef_[2] = {
    0x00, 0xff
};

/**
 * Tests that fromHostToTargetByteOrder works correctly.
 */
inline void
Swapper00Test::testFromHostToTarget() {
    SwapperTestCore::fromHostToTarget();
}

/**
 * Tests that fromTargetToHostByteOrder works correctly.
 */
inline void
Swapper00Test::testFromTargetToHost() {
    SwapperTestCore::fromTargetToHost();
}

/**
 * Tests that fromHostToBigEndianByteOrder works correctly.
 */
inline void
Swapper00Test::testFromHostToBigEndian() {
    SwapperTestCore::fromHostToBigEndian();
}

/**
 * Tests that fromBigEndianToHostByteOrder works correctly.
 */
inline void
Swapper00Test::testFromBigEndianToHost() {
    SwapperTestCore::fromBigEndianToHost();
}

#endif
