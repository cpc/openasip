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
 * @file UniversalFunctionUnitTest.hh 
 *
 * A test suite for UniversalFunctionUnit.
 * 
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_UNIVERSAL_FUNCTION_UNIT_TEST_HH
#define TTA_UNIVERSAL_FUNCTION_UNIT_TEST_HH

#include <string>
#include <TestSuite.h>

#include "UniversalFunctionUnit.hh"
#include "UniversalMachine.hh"
#include "FUPort.hh"
#include "OperationPool.hh"

using std::string;
using namespace TTAMachine;

/**
 * Class that tests UniversalFunctionUnit class.
 */
class UniversalFunctionUnitTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testOperation();
    
private:
    FunctionUnit* uFU_;
};


/**
 * Called before each test.
 */
void
UniversalFunctionUnitTest::setUp() {
    uFU_ = UniversalMachine::instance().functionUnitNavigator().item(0);
}


/**
 * Called after each test.
 */
void
UniversalFunctionUnitTest::tearDown() {
}


/**
 * Tests the operation() methods.
 */
void
UniversalFunctionUnitTest::testOperation() {

    TS_ASSERT(uFU_->operationCount() == 0);
    TS_ASSERT(uFU_->portCount() == 0);
    TS_ASSERT_THROWS(uFU_->operation(0), OutOfRange);
    
    TS_ASSERT_THROWS(uFU_->operation("foobar"), InstanceNotFound);
    TS_ASSERT(uFU_->operationCount() == 0);

    const string ADD = "ADD";
    const string ABSF = "ABSF";

    HWOperation* add = uFU_->operation(ADD);
    TS_ASSERT(uFU_->operationCount() == 1);
    TS_ASSERT_EQUALS(uFU_->portCount(), 3);
    HWOperation* secondAdd = uFU_->operation(ADD);
    TS_ASSERT(add == secondAdd);
    
    uFU_->operation(ABSF);
    TS_ASSERT(uFU_->operationCount() == 2);
    TS_ASSERT_EQUALS(uFU_->portCount(), 5);

    BaseFUPort* oc32 = uFU_->port(UniversalFunctionUnit::OC_SETTING_PORT_32);
    BaseFUPort* oc64 = uFU_->port(UniversalFunctionUnit::OC_SETTING_PORT_64);

    TS_ASSERT(oc32->isOpcodeSetting());
    TS_ASSERT(oc64->isOpcodeSetting());
    TS_ASSERT(oc32->width() == 32);
    TS_ASSERT(oc64->width() == 64);
    OperationPool::cleanupCache();
}

#endif
