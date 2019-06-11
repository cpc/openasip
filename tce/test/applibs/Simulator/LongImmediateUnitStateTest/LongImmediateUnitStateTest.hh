/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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
