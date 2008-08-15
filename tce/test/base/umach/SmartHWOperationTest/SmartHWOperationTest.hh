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
 * @file SmartHWOperationTest.hh 
 *
 * A test suite for SmartHWOperationTest.
 * 
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
    OperationPool* opPool_;
    UniversalMachine* machine_;
    FunctionUnit* uFU_;
};


/**
 * Called before each test.
 */
void
SmartHWOperationTest::setUp() {
    opPool_ = new OperationPool();
    machine_ = new UniversalMachine(*opPool_);
    uFU_ = machine_->functionUnitNavigator().item(0);
}


/**
 * Called after each test.
 */
void
SmartHWOperationTest::tearDown() {
    delete machine_;
    delete opPool_;
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
