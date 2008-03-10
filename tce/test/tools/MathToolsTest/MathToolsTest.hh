/**
 * @file MathToolsTest.hh
 *
 * A test suite for MathTools class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MATHTOOLS_TEST_HH
#define TTA_MATHTOOLS_TEST_HH

#include <string>
#include <vector>

#include <TestSuite.h>
#include "MathTools.hh"


using std::string;
using std::vector;


/**
 * Implements the tests needed to verify correct operation of MathTools.
 */
class MathToolsTest : public CxxTest::TestSuite {

public:
    void testRequiredBits();
    void testRequiredBitsSigned();
    void testSignExtendTo();
    void testZeroExtendTo();
};


/**
 * Tests the requiredBits() method.
 */
void 
MathToolsTest::testRequiredBits() {
    TS_ASSERT_EQUALS(MathTools::requiredBits(0), 1);
    TS_ASSERT_EQUALS(MathTools::requiredBits(5), 3);   //       101
    TS_ASSERT_EQUALS(MathTools::requiredBits(128), 8); //  10000000
    TS_ASSERT_EQUALS(MathTools::requiredBits(256), 9); // 100000000
}

/**
 * Tests the requiredBitsSigned() method.
 */
void 
MathToolsTest::testRequiredBitsSigned() {
    TS_ASSERT_EQUALS(MathTools::requiredBitsSigned(0), 1);
    TS_ASSERT_EQUALS(MathTools::requiredBitsSigned(1), 2);    //        01
    TS_ASSERT_EQUALS(MathTools::requiredBitsSigned(3), 3);    //       011
    TS_ASSERT_EQUALS(MathTools::requiredBitsSigned(-3), 3);   //       101
    TS_ASSERT_EQUALS(MathTools::requiredBitsSigned(-128), 8); //  10000000
    TS_ASSERT_EQUALS(MathTools::requiredBitsSigned(256), 10); //0100000000
    TS_ASSERT_EQUALS(MathTools::requiredBitsSigned(257), 10); //0100000001
}

/**
 * Tests the signExtendTo() method.
 */
void 
MathToolsTest::testSignExtendTo() {
    TS_ASSERT_EQUALS(MathTools::signExtendTo(0, 1), 0);
    TS_ASSERT_EQUALS(MathTools::signExtendTo(1, 1), -1);
    TS_ASSERT_EQUALS(MathTools::signExtendTo(3, 3), 3);
    TS_ASSERT_EQUALS(MathTools::signExtendTo(-3, 3), -3);
    TS_ASSERT_EQUALS(MathTools::signExtendTo(-128, 8), -128);
    TS_ASSERT_EQUALS(MathTools::signExtendTo(257, 10), 257);
    TS_ASSERT_THROWS(MathTools::signExtendTo(1, 1000), OutOfRange);
}

/**
 * Tests the zeroExtendTo() method.
 */
void 
MathToolsTest::testZeroExtendTo() {
    TS_ASSERT_EQUALS(MathTools::zeroExtendTo(0, 1), 0);
    TS_ASSERT_EQUALS(MathTools::zeroExtendTo(-1, 1), 1);
    TS_ASSERT_EQUALS(MathTools::zeroExtendTo(1, 1), 1);
    TS_ASSERT_EQUALS(MathTools::zeroExtendTo(1, 2), 1);
    TS_ASSERT_EQUALS(MathTools::zeroExtendTo(2, 2), 2);
    TS_ASSERT_EQUALS(MathTools::zeroExtendTo(-2, 2), 2);
}


#endif
