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
 * @file MachineTesterTest.hh 
 * A test suite for MachineTester.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 */

#ifndef MachineTesterTest_HH
#define MachineTesterTest_HH

#include <string>

#include <TestSuite.h>
#include "MachineTester.hh"
#include "Bus.hh"
#include "Socket.hh"
#include "Bridge.hh"
#include "Segment.hh"
#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "RegisterFile.hh"
#include "ImmediateUnit.hh"
#include "ControlUnit.hh"
#include "AssocTools.hh"

using std::string;
using namespace TTAMachine;

/**
 * Tests the functionality of MachineTester class.
 */
class MachineTesterTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCanConnectSocketSegment();
    void testCanConnectSocketPort();
    void testCanBridge();
    void testValidComponentName();

private:
};


/**
 * Called before each test.
 */
void
MachineTesterTest::setUp() {
}


/**
 * Called after each test.
 */
void
MachineTesterTest::tearDown() {
}


/**
 * Tests the functionality of canConnect(Socket&, Segment&) function.
 */
void
MachineTesterTest::testCanConnectSocketSegment() {
    
    const string bus1Name = "bus1";
    const string bus2Name = "bus2";
    const string bus3Name = "bus3";
    const string bus4Name = "bus4";
    const string bus5Name = "bus5";
    const string bus6Name = "bus6";
    const string b1s1Name = "b1s1";
    const string b2s1Name = "b2s1";
    const string b3s1Name = "b3s1";
    const string b4s1Name = "b4s1";
    const string b5s1Name = "b5s1";
    const string b6s1Name = "b6s1";
    const string bridge1Name = "bridge1";
    const string bridge2Name = "bridge2";
    const string bridge3Name = "bridge3";
    const string bridge4Name = "bridge4";
    const string socket1Name = "socket1";
    const string socket2Name = "socket2";
    const string socket3Name = "socket3";
    const string socket4Name = "socket4";
    const string fuName = "fu";
    const string port1Name = "port1";
    const string port2Name = "port2";
    const string rf1Name = "rf1";
    const string rf2Name = "rf2";

    Machine* mach = new Machine();
    Bus* bus1 = new Bus(bus1Name, 32, 32, Machine::SIGN);
    new Segment(b1s1Name, *bus1);
    Bus* bus2 = new Bus(bus2Name, 32, 32, Machine::SIGN);
    new Segment(b2s1Name, *bus2);
    mach->addBus(*bus1);

    MachineTester tester(*mach);

    // test that unregistered components cannot be attached
    Socket* socket1 = new Socket(socket1Name);
    TS_ASSERT(!tester.canConnect(*socket1, *bus1->segment(0)));
    TS_ASSERT(tester.illegalRegistration());
    mach->addSocket(*socket1);
    TS_ASSERT(tester.canConnect(*socket1, *bus1->segment(0)));
    TS_ASSERT(!tester.canConnect(*socket1, *bus2->segment(0)));
    TS_ASSERT(tester.illegalRegistration());
    
    // check that equal connections cannot be made
    socket1->attachBus(*bus1->segment(0));
    TS_ASSERT(!tester.canConnect(*socket1, *bus1->segment(0)));
    TS_ASSERT(tester.connectionExists());

    // test the case that there is a socket that has no segment connections
    // but is connected to ports in such a way that its direction cannot be
    // set to either input or output
    delete mach;
    mach = new Machine();
    bus1 = new Bus(bus1Name, 32, 32, Machine::SIGN);
    new Segment(b1s1Name, *bus1);
    mach->addBus(*bus1);
    socket1 = new Socket(socket1Name);
    mach->addSocket(*socket1);
    socket1->attachBus(*bus1->segment(0));
    socket1->setDirection(Socket::OUTPUT);
    Socket* socket2 = new Socket(socket2Name);
    mach->addSocket(*socket2);
    socket2->attachBus(*bus1->segment(0));
    socket2->setDirection(Socket::INPUT);
    RegisterFile* regFile1 = new RegisterFile(
        rf1Name, 16, 16, 1, 1, 0, RegisterFile::NORMAL);
    Port* rf1Port = new RFPort(port1Name, *regFile1);
    mach->addRegisterFile(*regFile1);
    RegisterFile* regFile2 = new RegisterFile(
        rf2Name, 16, 16, 1, 1, 0, RegisterFile::NORMAL);
    Port* rf2Port = new RFPort(port1Name, *regFile2);
    mach->addRegisterFile(*regFile2);
    rf1Port->attachSocket(*socket1);
    rf2Port->attachSocket(*socket2);
    Socket* socket3 = new Socket(socket3Name);
    mach->addSocket(*socket3);
    rf1Port->attachSocket(*socket3);
    rf2Port->attachSocket(*socket3);
    TS_ASSERT(!tester.canConnect(*socket3, *bus1->segment(0)));
    delete mach;
}


/**
 * Tests the functionality of canConnect(Socket&, Port&) function.
 */
void
MachineTesterTest::testCanConnectSocketPort() {

    const string socket1Name = "socket1";
    const string socket2Name = "socket2";
    const string socket3Name = "socket3";
    const string bus1Name = "bus1";
    const string bus2Name = "bus2";
    const string bus3Name = "bus3";
    const string segment1Name = "segment1";
    const string rf1Name = "rf1";
    const string port1Name = "port1";
    const string port2Name = "port2";
    const string bridge1Name = "bridge1";
    const string iu1Name = "iu1";
    const string cuName = "cu";
    const string controlPortName = "control";
    
    Machine* mach = new Machine();
    MachineTester tester(*mach);

    // check illegal registration
    Socket* socket1 = new Socket(socket1Name);
    RegisterFile* regFile1 = new RegisterFile(
        rf1Name, 5, 16, 1, 1, 0, RegisterFile::NORMAL);
    Port* rf1Port1 = new RFPort(port1Name, *regFile1);
    TS_ASSERT(!tester.canConnect(*socket1, *rf1Port1));
    TS_ASSERT(tester.illegalRegistration());
    mach->addUnit(*regFile1);
    TS_ASSERT(!tester.canConnect(*socket1, *rf1Port1));
    TS_ASSERT(tester.illegalRegistration());
    mach->addSocket(*socket1);
    TS_ASSERT(tester.canConnect(*socket1, *rf1Port1));
    mach->removeUnit(*regFile1);
    TS_ASSERT(!tester.canConnect(*socket1, *rf1Port1));
    TS_ASSERT(tester.illegalRegistration());
    mach->addUnit(*regFile1);

    // check the case of fully connected port
    Socket* socket2 = new Socket(socket2Name);
    mach->addSocket(*socket2);
    rf1Port1->attachSocket(*socket1);
    rf1Port1->attachSocket(*socket2);
    Socket* socket3 = new Socket(socket3Name);
    mach->addSocket(*socket3);
    TS_ASSERT(!tester.canConnect(*socket3, *rf1Port1));
    TS_ASSERT(tester.maxConnections());

    // check the case of wrong direction of socket
    rf1Port1->detachSocket(*socket2);
    Bus* bus1 = new Bus(bus1Name, 32, 32, Machine::SIGN);
    new Segment(segment1Name, *bus1);
    mach->addBus(*bus1);
    socket1->attachBus(*bus1->segment(0));
    socket1->setDirection(Socket::INPUT);
    socket2->attachBus(*bus1->segment(0));
    socket2->setDirection(Socket::INPUT);
    TS_ASSERT(!tester.canConnect(*socket2, *rf1Port1));
    TS_ASSERT(tester.wrongSocketDirection());
    socket1->setDirection(Socket::OUTPUT);
    TS_ASSERT(tester.canConnect(*socket2, *rf1Port1));
    socket2->setDirection(Socket::OUTPUT);
    TS_ASSERT(!tester.canConnect(*socket2, *rf1Port1));
    TS_ASSERT(tester.wrongSocketDirection());

    // check the special cases
    ImmediateUnit* iu = new ImmediateUnit(iu1Name, 10, 32, 1, 0, Machine::SIGN);
    Port* iuPort = new RFPort(port1Name, *iu);
    mach->addUnit(*iu);
    TS_ASSERT(tester.canConnect(*socket2, *iuPort));
    iuPort->attachSocket(*socket2);
    socket1->setDirection(Socket::INPUT);
    TS_ASSERT(!tester.canConnect(*socket1, *iuPort));
    TS_ASSERT(tester.maxConnections());
    iuPort->detachSocket(*socket2);
    TS_ASSERT(!tester.canConnect(*socket1, *iuPort));
    TS_ASSERT(tester.wrongSocketDirection());

    delete mach;

    // check that port of register file cannot be connected to the same
    // socket
    mach = new Machine();
    regFile1 = new RegisterFile(
        rf1Name, 32, 32, 1, 1, 0, RegisterFile::NORMAL);
    mach->addRegisterFile(*regFile1);
    rf1Port1 = new RFPort(port1Name, *regFile1);
    Port* rf1Port2 = new RFPort(port2Name, *regFile1);
    socket1 = new Socket(socket1Name);
    mach->addSocket(*socket1);
    rf1Port1->attachSocket(*socket1);
    MachineTester tester2(*mach);
    TS_ASSERT(!tester2.canConnect(*socket1, *rf1Port2));
    TS_ASSERT(tester2.registerFilePortAlreadyConnected());
    delete mach;
}


/**
 * Tests the functionality of canBridge function.
 */
void MachineTesterTest::testCanBridge() {

    const string bus1Name = "bus1";
    const string bus2Name = "bus2";
    const string bus3Name = "bus3";
    const string bus4Name = "bus4";
    const string b1s1Name = "b1s1";
    const string b2s1Name = "b2s1";
    const string b3s1Name = "b3s1";
    const string b4s1Name = "b4s1";
    const string socket1Name = "socket1";
    const string socket2Name = "socket2";
    const string socket3Name = "socket3";
    const string socket4Name = "socket4";
    const string bridge1Name = "bridge1";
    const string bridge2Name = "bridge2";
    const string fuName = "fu";
    const string fuPort1Name = "fuPort1";
    const string fuPort2Name = "fuPort2";

    Machine* mach = new Machine();
    MachineTester tester(*mach);

    Bus* bus1 = new Bus(bus1Name, 32, 16, Machine::SIGN);
    new Segment(b1s1Name, *bus1);
    Bus* bus2 = new Bus(bus2Name, 32, 16, Machine::SIGN);
    new Segment(b2s1Name, *bus2);

    TS_ASSERT(!tester.canBridge(*bus1, *bus1));
    TS_ASSERT(!tester.canBridge(*bus1, *bus2));
    TS_ASSERT(tester.illegalRegistration());

    mach->addBus(*bus1);
    TS_ASSERT(!tester.canBridge(*bus1, *bus2));
    TS_ASSERT(tester.illegalRegistration());
    mach->addBus(*bus2);
    TS_ASSERT(tester.canBridge(*bus1, *bus2));

    // create loop
    new Bridge(bridge1Name, *bus1, *bus2);
    TS_ASSERT(tester.canBridge(*bus2, *bus1));
    TS_ASSERT(!tester.canBridge(*bus1, *bus2));
    TS_ASSERT(tester.connectionExists());
    Bus* bus3 = new Bus(bus3Name, 32, 16, Machine::SIGN);
    new Segment(b3s1Name, *bus3);
    mach->addBus(*bus3);
    new Bridge(bridge2Name, *bus2,  *bus3);
    TS_ASSERT(!tester.canBridge(*bus1, *bus3));
    TS_ASSERT(tester.loop());
    TS_ASSERT(!tester.canBridge(*bus3, *bus1));
    TS_ASSERT(tester.loop());

    // create branch
    Bus* bus4 = new Bus(bus4Name, 32, 16, Machine::SIGN);
    new Segment(b4s1Name, *bus4);
    mach->addBus(*bus4);
    TS_ASSERT(!tester.canBridge(*bus2, *bus4));
    TS_ASSERT(tester.branchedBus() == bus2);

    delete mach;
}


/**
 * Tests the functionality of isValidComponentName method.
 */
void
MachineTesterTest::testValidComponentName() {

    const string invalid1 = "4fg";
    const string invalid2 = " gf";
    const string invalid3 = "g,";
    const string invalid4 = "g-";
    const string invalid5 = "fd.f";
    const string valid1 = "a9f";
    const string valid2 = "H3g_:";
    
    TS_ASSERT(!MachineTester::isValidComponentName(invalid1));
    TS_ASSERT(!MachineTester::isValidComponentName(invalid2));
    TS_ASSERT(!MachineTester::isValidComponentName(invalid3));
    TS_ASSERT(!MachineTester::isValidComponentName(invalid4));
    TS_ASSERT(!MachineTester::isValidComponentName(invalid5));
    TS_ASSERT(MachineTester::isValidComponentName(valid1));
    TS_ASSERT(MachineTester::isValidComponentName(valid2));
}

#endif
