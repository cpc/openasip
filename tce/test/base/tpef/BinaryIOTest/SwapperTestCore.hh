/**
 * @file SwapperTestCore.hh
 *
 * Implementation of SwapperTestCore class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
 * @note reviewed 23 October 2003 by am, jn, ll, pj
 */

#ifndef TTA_SWAPPER_TEST_CORE_HH
#define TTA_SWAPPER_TEST_CORE_HH

#include "Swapper.hh"

using namespace TPEF;

/**
 * Implements the actual test functions which verify correct operation
 * of Swapper.
 */
class SwapperTestCore {
public:
    static void fromHostToTarget();
    static void fromTargetToHost();
    static void fromHostToBigEndian();
    static void fromBigEndianToHost();

private:
    /// A test Word.
    static const Word word_;
    /// Test word in swapped byte order.
    static const Word wordSwapped_;
    /// Test Word bytes in host/target configuration dependent byte order.
    static const Byte wordBytesRef_[4];
    /// Test Word bytes in big-endian byte order.
    static const Byte wordBytesBE_[4];
    
    /// A test HalfWord.
    static const HalfWord halfWord_;
    /// Test HalfWord in swapped byte order.
    static const HalfWord halfWordSwapped_;
    /// Test HalfWord bytes in host/target configuration dependent byte order.
    static const Byte halfWordBytesRef_[2];
    /// Test HalfWord bytes in big-endian byte order.
    static const Byte halfWordBytesBE_[2];
};

const Byte SwapperTestCore::wordBytesBE_[4] = {
    0x01, 0x02, 0x03, 0x04
};

const Byte SwapperTestCore::halfWordBytesBE_[2] = {
    0xff, 0x00
};

/**
 * Tests that fromHostToTargetByteOrder works correctly.
 */
inline void
SwapperTestCore::fromHostToTarget() {

    Byte wBytes[sizeof(Word)];
    Byte hwBytes[sizeof(HalfWord)];
    
    Word testWord = TEST_HOST_BIGENDIAN ? wordSwapped_ : word_;
    HalfWord testHalfWord = TEST_HOST_BIGENDIAN ? halfWordSwapped_ :
                                                  halfWord_;
    
    Swapper::fromHostToTargetByteOrder(testWord, wBytes);
    Swapper::fromHostToTargetByteOrder(testHalfWord, hwBytes);
    
    for (Word i = 0; i < sizeof(Word); i++) {
        TS_ASSERT_EQUALS(wBytes[i], wordBytesRef_[i]);
    }
    for (Word i = 0; i < sizeof(HalfWord); i++) {
        TS_ASSERT_EQUALS(hwBytes[i], halfWordBytesRef_[i]);
    }
}

/**
 * Tests that fromTargetToHostByteOrder works correctly.
 */
inline void
SwapperTestCore::fromTargetToHost() {

    Byte wBytes[sizeof(Word)];
    for (Word i = 0; i < sizeof(Word); i++) {
        wBytes[i] = wordBytesRef_[i];
    }
    
    Byte hwBytes[sizeof(HalfWord)];
    for (Word i = 0; i < sizeof(HalfWord); i++) {
        hwBytes[i] = halfWordBytesRef_[i];
    }
    
    Word testWord;
    HalfWord testHalfWord;
    
    Swapper::fromTargetToHostByteOrder(wBytes, testWord);
    Swapper::fromTargetToHostByteOrder(hwBytes, testHalfWord);
    
    Word refWord = TEST_HOST_BIGENDIAN ? wordSwapped_ : word_;
    HalfWord refHalfWord = TEST_HOST_BIGENDIAN ? halfWordSwapped_ :
                                                 halfWord_;
    
    TS_ASSERT_EQUALS(testWord, refWord);
    TS_ASSERT_EQUALS(testHalfWord, refHalfWord);
}

/**
 * Tests that fromHostToBigEndianByteOrder works correctly.
 */
inline void
SwapperTestCore::fromHostToBigEndian() {

    Byte wBytes[sizeof(Word)];
    Byte hwBytes[sizeof(HalfWord)];
    
    
    Word testWord = TEST_HOST_BIGENDIAN ? wordSwapped_ : word_;
    HalfWord testHalfWord = TEST_HOST_BIGENDIAN ? halfWordSwapped_ :
                                                  halfWord_;
    
    Swapper::fromHostToBigEndianByteOrder(testWord, wBytes);
    Swapper::fromHostToBigEndianByteOrder(testHalfWord, hwBytes);
    
    for (Word i = 0; i < sizeof(Word); i++) {
        TS_ASSERT_EQUALS(wBytes[i], wordBytesBE_[i]);
    }
    for (Word i = 0; i < sizeof(HalfWord); i++) {
        TS_ASSERT_EQUALS(hwBytes[i], halfWordBytesBE_[i]);
    }
}

/**
 * Tests that fromBigEndianToHostByteOrder works correctly.
 */
inline void
SwapperTestCore::fromBigEndianToHost() {

    Byte wBytes[sizeof(Word)];
    for (Word i = 0; i < sizeof(Word); i++) {
        wBytes[i] = wordBytesBE_[i];
    }
    
    Byte hwBytes[sizeof(HalfWord)];
    for (Word i = 0; i < sizeof(HalfWord); i++) {
        hwBytes[i] = halfWordBytesBE_[i];
    }
    
    Word testWord;
    HalfWord testHalfWord;
    
    Swapper::fromBigEndianToHostByteOrder(wBytes, testWord);
    Swapper::fromBigEndianToHostByteOrder(hwBytes, testHalfWord);
    
    Word refWord = TEST_HOST_BIGENDIAN ? wordSwapped_ : word_;
    HalfWord refHalfWord = TEST_HOST_BIGENDIAN ? halfWordSwapped_ :
                                                 halfWord_;
    
    TS_ASSERT_EQUALS(testWord, refWord);
    TS_ASSERT_EQUALS(testHalfWord, refHalfWord);
}

#endif
