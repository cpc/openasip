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
 * @file FUPortTest.hh 
 * A test suite for FUPort.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef FUPortTest_HH
#define FUPortTest_HH

#include <string>

#include <TestSuite.h>
#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "Machine.hh"
#include "Guard.hh"

using std::string;
using namespace TTAMachine;

const string fuName = "fu";
const string port1Name = "port1";
const string port2Name = "port2";
const string port3Name = "port3";
const string port4Name = "port4";
const string bus1Name = "bus1";
const string op1Name = "op1";

/**
 * Tests the functionality of FUPort class.
 */
class FUPortTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();
    void testSetTriggering();
    void testDeletion();

private:
};


/**
 * Called before each test.
 */
void
FUPortTest::setUp() {
}


/**
 * Called after each test.
 */
void
FUPortTest::tearDown() {
}


/**
 * Tests adding a port.
 */
void
FUPortTest::testCreation() {

    FunctionUnit fu(fuName);

    // ports cannot be opcode-setting if not triggering
    TS_ASSERT_THROWS(new FUPort(port1Name, 32, fu, false, true), 
                     IllegalParameters);
    TS_ASSERT(!fu.hasPort(port1Name));

    TS_ASSERT_THROWS_NOTHING(new FUPort(port1Name, 32, fu, true, true));
    TS_ASSERT(fu.hasPort(port1Name));

    // cannot add a port with the same name of an existing port...
    TS_ASSERT_THROWS(new FUPort(port1Name, 32, fu, false, false),
                     ComponentAlreadyExists);
    // ...even if the characteristics of the port as different
    TS_ASSERT_THROWS(new FUPort(port1Name, 24, fu, true, false),
                     ComponentAlreadyExists);

    new FUPort(port2Name, 32, fu, false, false);
    TS_ASSERT(fu.hasPort(port2Name));

    // cannot add more than one opcode-setting port
    TS_ASSERT_THROWS(new FUPort(port3Name, 32, fu, true, true),
                     ComponentAlreadyExists);
    TS_ASSERT(!fu.hasPort(port3Name));

    // bit width of ports must be greater than zero
    TS_ASSERT_THROWS(new FUPort(port3Name, 0, fu, false, false),
                     OutOfRange);
    TS_ASSERT(!fu.hasPort(port3Name));
}


/**
 * Tests the setTriggering (setOpcodeSetting) method.
 */
void
FUPortTest::testSetTriggering() {

    FunctionUnit fu(fuName);
    FUPort* port1 = new FUPort(port1Name, 32, fu, true, true);
    FUPort* port2 = new FUPort(port2Name, 32, fu, false, false);

    // if no port is opcode-setting, any port can be made so
    port2->setTriggering(true);
    TS_ASSERT(port2->isOpcodeSetting());
    // being opcode-setting implies being a triggering port, too
    TS_ASSERT(port2->isTriggering());

    port1->setTriggering(true);
    // Triggering port is always the opcode setting port in TCE v1.0 and
    // only one port can trigger in TCE v1.0.
    TS_ASSERT(port1->isOpcodeSetting());
    TS_ASSERT(port1->isTriggering());

    TS_ASSERT(!port2->isOpcodeSetting());
    TS_ASSERT(!port2->isTriggering());

    delete port1;
    delete port2;
}


/**
 * Tests the functionality of deletion of a function unit port.
 */
void
FUPortTest::testDeletion() {

    FunctionUnit* fu = new FunctionUnit(fuName);
    HWOperation* op1 = new HWOperation(op1Name, *fu);

    // test binding between nontriggering port and operation input-output
    FUPort* port1 = new FUPort(port1Name, 32, *fu, false, false);
    op1->bindPort(1, *port1);
    TS_ASSERT(op1->port(1) == port1);
    // binding is automatically reset when a port is deleted
    delete port1;
    port1 = NULL;
    TS_ASSERT(op1->port(1) == NULL);

    // test binding between opcode-setting port and operation input-output
    FUPort* port3 = new FUPort(port1Name, 32, *fu, true, true);
    op1->bindPort(1, *port3);
    TS_ASSERT(op1->port(1) == port3);
    // binding is automatically reset when a port is deleted
    delete port3;
    port3 = NULL;
    TS_ASSERT(op1->port(1) == NULL);

    // create bus and port guard and test that guard is automatically
    // deleted when the port is deleted
    Machine machine;
    FUPort* port4 = new FUPort(port1Name, 32, *fu, false, false);
    FUPort* port2 = new FUPort(port2Name, 32, *fu, false, false);
    machine.addFunctionUnit(*fu);
    Bus* bus1 = new Bus(bus1Name, 32, 32, Machine::SIGN);
    machine.addBus(*bus1);
    new PortGuard(false, *port4, *bus1);
    new PortGuard(false, *port2, *bus1);
    TS_ASSERT(bus1->guardCount() == 2);
    delete port4;
    port4 = NULL;
    TS_ASSERT(bus1->guardCount() == 1);
    delete port2;
    port2 = NULL;
    TS_ASSERT(bus1->guardCount() == 0);
}


#endif
