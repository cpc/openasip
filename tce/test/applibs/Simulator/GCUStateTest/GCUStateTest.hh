/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file GCUStateTest.hh
 * 
 * A test suite for GCUState.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005,2010 (pjaaskel-no.spam-cs.tut.fi)
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

    GCUState gcu(3, 4, 3);

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

    GCUState gcu(2, 4, 3);
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
