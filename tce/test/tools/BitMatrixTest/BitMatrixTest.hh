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
 * @file BitMatrixTest.hh
 *
 * A test suite for BitMatrix
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
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
