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
 * @file SmartHWOperationTest.hh 
 *
 * A test suite for SmartHWOperationTest.
 * 
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SMART_HW_OPERATION_TEST_HH
#define TTA_SMART_HW_OPERATION_TEST_HH

#include <string>
#include <TestSuite.h>

#include "OperationPool.hh"
#include "UniversalMachine.hh"
#include "HWOperation.hh"
#include "FUPort.hh"

using std::string;
using namespace TTAMachine;

/**
 * Class that tests SmartHWOperation class.
 */
class SmartHWOperationTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testOperandBindings();

private:
    FunctionUnit* uFU_;
};


/**
 * Called before each test.
 */
void
SmartHWOperationTest::setUp() {
    uFU_ = UniversalMachine::instance().functionUnitNavigator().item(0);
}


/**
 * Called after each test.
 */
void
SmartHWOperationTest::tearDown() {
}


/**
 * Tests the automatic operand bindings.
 */
void
SmartHWOperationTest::testOperandBindings() {

    const string ADD = "ADD";
    const string ABSF = "ABSF";
    const string OPTEST = "OPTEST";

    HWOperation* add = uFU_->operation(ADD);

    FUPort* port1 = add->port(1);
    FUPort* port2 = add->port(2);
    FUPort* port3 = add->port(3);
    
    TS_ASSERT(add->port(4) == NULL);
    TS_ASSERT(add->port(5) == NULL);

    TS_ASSERT(port1->inputSocket() != NULL);
    TS_ASSERT(port2->inputSocket() != NULL);
    TS_ASSERT(port3->outputSocket() != NULL);
    
    TS_ASSERT(port2->isOpcodeSetting());

    HWOperation* absf = uFU_->operation(ABSF);

    port1 = absf->port(1);
    port2 = absf->port(2);

    TS_ASSERT(port1->inputSocket() != NULL);
    TS_ASSERT(port2->outputSocket() != NULL);
    TS_ASSERT(port1->isOpcodeSetting());

    HWOperation* opTest = uFU_->operation(OPTEST);
    
    port1 = opTest->port(1);
    port2 = opTest->port(2);
    port3 = opTest->port(3);
    
    TS_ASSERT(port1->inputSocket() != NULL);
    TS_ASSERT(port2->inputSocket() != NULL);
    TS_ASSERT(port3->inputSocket() != NULL);
    TS_ASSERT(!port2->isOpcodeSetting());
    TS_ASSERT(port3->isOpcodeSetting());

    OperationPool::cleanupCache();
}

#endif
