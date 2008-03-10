/** 
 * @file GCUStateTest.hh
 * 
 * A test suite for GCUState.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef GCU_STATE_TEST_HH
#define GCU_STATE_TEST_HH

#include <TestSuite.h>
#include <iostream>

#include "GCUState.hh"
#include "TransportPipeline.hh"
#include "InputPortState.hh"
#include "TriggeringInputPortState.hh"
#include "OpcodeSettingVirtualInputPortState.hh"
#include "OperationPool.hh"
#include "Operation.hh"
#include "GlobalLock.hh"
#include "Application.hh"
#include "BaseType.hh"

using std::cout;
using std::endl;

/**
 * Class for testing GCUState.
 */
class GCUStateTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testPipeline();
    void testOperations();
};


/**
 * Called before each test.
 */
void
GCUStateTest::setUp() {
}


/**
 * Called after each test.
 */
void
GCUStateTest::tearDown() {
}

/**
 * Test that the GCUState pipeline works as it should.
 */
void
GCUStateTest::testPipeline() {

    GlobalLock lock;
    GCUState gcu(3, 4, lock);

    gcu.advanceClock(); // cycle 1

    TS_ASSERT_EQUALS(static_cast<int>(gcu.programCounter()), 0);
    TS_ASSERT_EQUALS(gcu.returnAddressRegister().intValue(), 0);

    InstructionAddress value = 5;
    gcu.setProgramCounter(value);
    gcu.setReturnAddress();

    gcu.advanceClock(); // cycle 2

    TS_ASSERT_EQUALS(static_cast<int>(gcu.programCounter()), 0);
    TS_ASSERT_EQUALS(gcu.returnAddressRegister().intValue(), 3);

    gcu.advanceClock(); // cycle 3
   
    TS_ASSERT_EQUALS(static_cast<int>(gcu.programCounter()), 0);
    
    gcu.advanceClock(); // cycle 4

    TS_ASSERT_EQUALS(static_cast<int>(gcu.programCounter()), 5);

    gcu.advanceClock(); // cycle 5
}

/**
 * Test that real operations change the GCUState as they should.
 */
void
GCUStateTest::testOperations() {

    GlobalLock lock;
    GCUState gcu(2, 4, lock);
    TransportPipeline sysPipeline(gcu);
    TransportPipeline jumpPipeline(gcu);
    OperationPool pool;
    Operation& jump = pool.operation("JUMP");

    InputPortState input1(gcu, 32);
    TriggeringInputPortState input2(gcu, 32);
    OpcodeSettingVirtualInputPortState virtual2(jump, gcu, input2);
    gcu.addInputPortState(input1);
    gcu.addInputPortState(input2);
    gcu.addInputPortState(virtual2);

    TS_ASSERT_THROWS_NOTHING(jumpPipeline.addBinding(1, input2));
    TS_ASSERT_THROWS_NOTHING(gcu.addOperationExecutor(jumpPipeline, jump));
    
    // cycle 1, ready in cycle 3
    SimValue value1(32);
    value1 = 10;
    SimValue value2(32);
    value2 = 20;
    input1.setValue(value1);

    gcu.endClock();
    gcu.advanceClock();
    
    // cycle 2, ready in cycle 4
    TS_ASSERT_EQUALS(static_cast<int>(gcu.programCounter()), 0);
    TS_ASSERT_EQUALS(gcu.returnAddressRegister().intValue(), 0);

    value1 = 50;
    virtual2.setValue(value1);
    
    gcu.endClock();
    gcu.advanceClock();

    OperationPool::cleanupCache();
}

#endif
