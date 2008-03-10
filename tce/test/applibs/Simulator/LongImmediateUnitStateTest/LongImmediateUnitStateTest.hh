/** 
 * @file LongImmediateUnitStateTest.hh
 * 
 * A test suite for LongImmediateUnitState.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
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
