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
 * @file SimValueTest.hh
 * 
 * A test suite for SimValue.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
 */

#ifndef SIM_VALUE_TEST_HH
#define SIM_VALUE_TEST_HH

#include <TestSuite.h>
#include "SimValue.hh"

class SimValueTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testAssignment();

private:
};


/**
 * Called before each test.
 */
void
SimValueTest::setUp() {
}


/**
 * Called after each test.
 */
void
SimValueTest::tearDown() {
}

/**
 * Tests assignment operators.
 */
void
SimValueTest::testAssignment() {
    
    SimValue val(10);
    SIntWord test1 = 12;

    TS_ASSERT_EQUALS(val.width(), 10);
    TS_ASSERT_EQUALS(val.isActive(), false);

    val = test1;
    val.setActive();

    TS_ASSERT_EQUALS(val.sIntWordValue(), test1);
    TS_ASSERT_EQUALS(val.isActive(), true);

    FloatWord test5 = 4.4;

    val.clearActive();
    TS_ASSERT_EQUALS(val.isActive(), false);

    val = test5;
    val.setActive();

    TS_ASSERT_EQUALS(val.floatWordValue(), test5);
    TS_ASSERT_EQUALS(val.isActive(), true);

    DoubleWord test6 = 54.332;

    val.clearActive();
    TS_ASSERT_EQUALS(val.isActive(), false);

    val = test6;
    val.setActive();
    TS_ASSERT_EQUALS(val.doubleWordValue(), test6);
    TS_ASSERT_EQUALS(val.isActive(), true);

    SimValue val2(10);

    val2 = val;
    val2.setActive();
    TS_ASSERT_EQUALS(val2.doubleWordValue(), test6);
    TS_ASSERT_EQUALS(val2.isActive(), true);

    // Test unsigned bit masking.
    SimValue val3(3);
    val3 = 8; // (0..01)000
    unsigned int test = 0;
    TS_ASSERT_EQUALS(val3.unsignedValue(), test);
    val3 = 7; // (0..0)111
    test = 7;
    TS_ASSERT_EQUALS(val3.unsignedValue(), test);

    // Test signed value extension.
    val3 = -4; // (1..1)100
    TS_ASSERT_EQUALS(val3.intValue(), -4);
    val3 = -5; // (1..1)011 
    TS_ASSERT_EQUALS(val3.intValue(), 3);
    val3 = 3; // (0..0)011
    TS_ASSERT_EQUALS(val3.intValue(), 3);
    val3 = 4; //(0..0)100
    TS_ASSERT_EQUALS(val3.intValue(), -4);

    SimValue val4(64);
    val4 = 0x80000001;
    TS_ASSERT_EQUALS(val4.intValue(), -2147483647);
    TS_ASSERT_EQUALS(val4.unsignedValue(), 0x80000001);
}

#endif
