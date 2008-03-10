/**
 * @file BitMatrixTest.hh
 *
 * A test suite for BitMatrix
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
 */

#ifndef TTA_BIT_MATRIX_TEST_HH
#define TTA_BIT_MATRIX_TEST_HH

#include <TestSuite.h>
#include "BitMatrix.hh"
#include "Application.hh"

/**
 * Implements the tests needed to verify correct operation of BitMatrix.
 */
class BitMatrixTest : public CxxTest::TestSuite {
public:
    void testBasic();
};

/**
 * Tests the basic operations on bit matrix.
 */
void
BitMatrixTest::testBasic() {

    BitMatrix matrix(40, 34, false);
    TS_ASSERT_EQUALS(matrix.bitAt(33, 33), false);

    matrix.setBit(33, 33, true);
    TS_ASSERT_EQUALS(matrix.bitAt(33, 33), true);

    matrix.shiftLeft();

    TS_ASSERT_EQUALS(matrix.bitAt(32, 33), true);
    TS_ASSERT_EQUALS(matrix.bitAt(33, 33), false);

    matrix.shiftLeft();

    TS_ASSERT_EQUALS(matrix.bitAt(31, 33), true);
    TS_ASSERT_EQUALS(matrix.bitAt(32, 33), false);

    // test the copy constructor
    BitMatrix another(matrix);

    TS_ASSERT_EQUALS(another.bitAt(31, 33), true);
    TS_ASSERT_EQUALS(another.bitAt(32, 33), false);    

    another.shiftLeft();
    another.orWith(matrix);

    TS_ASSERT_EQUALS(another.bitAt(30, 33), true);
    TS_ASSERT_EQUALS(another.bitAt(31, 33), true);
    TS_ASSERT_EQUALS(another.bitAt(32, 33), false);    

    // test the equality operator
    TS_ASSERT_DIFFERS(matrix, another);

    another.setBit(30, 33, false);

    TS_ASSERT_EQUALS(another.bitAt(30, 33), false);
    TS_ASSERT_EQUALS(matrix, another);

    TS_ASSERT(matrix.conflictsWith(matrix));
    TS_ASSERT(matrix.conflictsWith(another));

    TS_ASSERT_EQUALS(another.bitAt(30, 33), false);
    TS_ASSERT_EQUALS(matrix, another);

    TS_ASSERT_EQUALS(another.bitAt(30, 33), false);
    TS_ASSERT_EQUALS(another.bitAt(31, 33), true);
    TS_ASSERT_EQUALS(another.bitAt(32, 33), false);
    another.shiftLeft();
    TS_ASSERT_EQUALS(another.bitAt(29, 33), false);
    TS_ASSERT_EQUALS(another.bitAt(30, 33), true);
    TS_ASSERT_EQUALS(another.bitAt(31, 33), false);
    TS_ASSERT(!matrix.conflictsWith(another));

    another.setAllToZero();

    TS_ASSERT_EQUALS(another.bitAt(29, 33), false);
    TS_ASSERT_EQUALS(another.bitAt(25, 30), false);
    TS_ASSERT_EQUALS(another.bitAt(14, 10), false);
    TS_ASSERT_EQUALS(another.bitAt(1, 5), false);

    another.setAllToOne();

    TS_ASSERT_EQUALS(another.bitAt(29, 33), true);
    TS_ASSERT_EQUALS(another.bitAt(25, 30), true);
    TS_ASSERT_EQUALS(another.bitAt(14, 10), true);
    TS_ASSERT_EQUALS(another.bitAt(1, 5), true);


}

#endif
