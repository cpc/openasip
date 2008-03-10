/**
 * @file Swapper11Test.hh
 *
 * A test suite for Swapper. Host and target machines are defined as
 * big-endian.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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
