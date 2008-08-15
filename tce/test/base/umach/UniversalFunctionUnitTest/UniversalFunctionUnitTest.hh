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
 * @file UniversalFunctionUnitTest.hh 
 *
 * A test suite for UniversalFunctionUnit.
 * 
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
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
    OperationPool* opPool_;
    UniversalMachine* machine_;
    FunctionUnit* uFU_;
};


/**
 * Called before each test.
 */
void
UniversalFunctionUnitTest::setUp() {
    opPool_ = new OperationPool();
    machine_ = new UniversalMachine(*opPool_);
    uFU_ = machine_->functionUnitNavigator().item(0);
}


/**
 * Called after each test.
 */
void
UniversalFunctionUnitTest::tearDown() {
    delete machine_;
    delete opPool_;
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
