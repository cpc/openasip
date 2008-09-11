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
 * @file LongImmediateUnitStateTest.hh
 * 
 * A test suite for LongImmediateUnitState.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef LONG_IMMEDIATE_UNIT_STATE_TEST_HH
#define LONG_IMMEDIATE_UNIT_STATE_TEST_HH

#include <TestSuite.h>
#include "LongImmediateUnitState.hh"

/**
 * Class for testing LongImmediateUnitState.
 */
class LongImmediateUnitStateTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    
    void testValueUpdating();
private:
};


/**
 * Called before each test.
 */
void
LongImmediateUnitStateTest::setUp() {
}


/**
 * Called after each test.
 */
void
LongImmediateUnitStateTest::tearDown() {
}

/**
 * Test that values are updated in registers as they should.
 */
void
LongImmediateUnitStateTest::testValueUpdating() {

    // latency doesn't affect at all, it's fixed
    LongImmediateUnitState immediates(100, 1, "g-unit", 32, true);
    
    SimValue value(32);
    
    // cycle one
    value = 5;
    immediates.setRegisterValue(10, value);
    
    immediates.endClock();
    immediates.advanceClock();

    // cycle two
    TS_ASSERT_EQUALS(immediates.registerValue(10), value);
    
    value = 10;
    immediates.setRegisterValue(52, value);
    
    immediates.endClock();
    immediates.advanceClock();

    // cycle three
    TS_ASSERT_EQUALS(immediates.registerValue(10), 5);
    TS_ASSERT_EQUALS(immediates.registerValue(52), value);

    immediates.endClock();
    immediates.advanceClock();

    // cycle four
    TS_ASSERT_EQUALS(immediates.registerValue(10), 5);
    TS_ASSERT_EQUALS(immediates.registerValue(52), value);

    immediates.endClock();
    immediates.advanceClock();
    
    // cycle five
    TS_ASSERT_EQUALS(immediates.registerValue(10), 5);
    TS_ASSERT_EQUALS(immediates.registerValue(52), value);
}

#endif
