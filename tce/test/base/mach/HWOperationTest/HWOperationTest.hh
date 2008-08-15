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
 * @file HWOperationTest.hh 
 * A test suite for HWOperation.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef HWOperationTest_HH
#define HWOperationTest_HH

#include <string>

#include <TestSuite.h>
#include "FUPort.hh"
#include "HWOperation.hh"
#include "FunctionUnit.hh"
#include "ExecutionPipeline.hh"

using std::string;
using namespace TTAMachine;

const string fuName = "fu";
const string port1Name = "port1";
const string port2Name = "port2";
const string opName = "op";
const string opNAME = "OP";
const string op2Name = "op2";
const string op2NAME = "OP2";

/**
 * Tests the functionality of HWOperation class.
 */
class HWOperationTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testConstructor();
    void testSetName();
    void testBindPort();

private:
    FunctionUnit* fu_;
};


/**
 * Called before each test.
 */
void
HWOperationTest::setUp() {
    fu_ = new FunctionUnit(fuName);
    new FUPort(port1Name, 32, *fu_, true, true);
    new FUPort(port2Name, 32, *fu_, false, false);
    new HWOperation(opName, *fu_);
}


/**
 * Called after each test.
 */
void
HWOperationTest::tearDown() {
    delete fu_;
}


/**
 * Tests the constructor of HWOperation class.
 */
void
HWOperationTest::testConstructor() {

    TS_ASSERT_THROWS(new HWOperation(opName, *fu_), ComponentAlreadyExists);
    TS_ASSERT_THROWS(new HWOperation(opNAME, *fu_), ComponentAlreadyExists);

    const string invalidName = "invalid name";
    TS_ASSERT_THROWS(new HWOperation(invalidName, *fu_), InvalidName);

    ObjectState* opState = fu_->operation(0)->saveState();
    TS_ASSERT_THROWS(
        new HWOperation(opState, *fu_), ObjectStateLoadingException);
    opState->setAttribute(HWOperation::OSKEY_NAME, invalidName);
    TS_ASSERT_THROWS(
        new HWOperation(opState, *fu_), ObjectStateLoadingException);
    
    const string validName = "valid";
    opState->setAttribute(HWOperation::OSKEY_NAME, validName);
    TS_ASSERT_THROWS_NOTHING(new HWOperation(opState, *fu_));
    delete opState;
}


/**
 * Tests setting the name of the operation.
 */
void
HWOperationTest::testSetName() {
    HWOperation* op = fu_->operation(opName);
    TS_ASSERT(op->name() == opName);
    op->setName(op2Name);
    TS_ASSERT(op->name() == op2Name);
    new HWOperation(opName, *fu_);
    TS_ASSERT_THROWS(op->setName(opName), ComponentAlreadyExists);
    TS_ASSERT(op->name() == op2Name);
    op->setName(op2NAME);
    TS_ASSERT(op->name() == op2Name);
}


/**
 * Tests binding an operand to a port of the function unit.
 */
void
HWOperationTest::testBindPort() {

    HWOperation* op1 = fu_->operation(0);
    FUPort* port1 = fu_->operationPort(0);
    FUPort* port2 = fu_->operationPort(1);

    op1->bindPort(1, *port1);
    op1->bindPort(2, *port2);
    
    TS_ASSERT(op1->port(1) == port1);
    TS_ASSERT(op1->port(2) == port2);
    
    TS_ASSERT_THROWS(op1->bindPort(2, *port1), ComponentAlreadyExists);

    TS_ASSERT(op1->io(*port1) == 1);
    TS_ASSERT(op1->io(*port2) == 2);

    FUPort* port3 = new FUPort("port3", 32, *fu_, false, false);
    TS_ASSERT_THROWS(op1->io(*port3), InstanceNotFound);

    op1->unbindPort(*port1);
    TS_ASSERT(op1->port(1) == NULL);
    
    FunctionUnit* fu2 = new FunctionUnit("fu2");
    FUPort* fu2Port = new FUPort("fu2Port", 32, *fu2, false, false);
    TS_ASSERT_THROWS(op1->bindPort(3, *fu2Port), IllegalRegistration);

    delete fu2;
}

#endif
