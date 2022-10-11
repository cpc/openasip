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
 * @file RFPortTest.hh 
 * A test suite for RFPort.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef RFPortTest_HH
#define RFPortTest_HH

#include <string>

#include <TestSuite.h>
#include "Machine.hh"
#include "Port.hh"
#include "RegisterFile.hh"
#include "Segment.hh"
#include "ObjectState.hh"

using std::string;
using namespace TTAMachine;

const string port1Name = "port1";
const string port2Name = "port2";
const string port3Name = "port3";
const string socket1Name = "socket1";
const string socket2Name = "socket2";
const string bus1Name = "bus1";
const string seg1Name = "seg1";
const int registerWidth = 32;

/**
 * Tests the functionality of RFPort class.
 */
class RFPortTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();
    void testSetName();
    void testWidth();
    void testSocketConnections();
    void testLoadState();

private:
    /// The machine used in tests.
    Machine* mach_;
    /// The register file used in tests.
    RegisterFile* rf1_;
};


/**
 * Called before each test.
 */
void
RFPortTest::setUp() {
    mach_ = new Machine();
    rf1_ = new RegisterFile(
        "rf1", 30, registerWidth, 1, 1, 0, RegisterFile::NORMAL);
    mach_->addRegisterFile(*rf1_);
}


/**
 * Called after each test.
 */
void
RFPortTest::tearDown() {
    delete mach_;
}


/**
 * Tests creation of a port.
 */
void
RFPortTest::testCreation() {

    TS_ASSERT_THROWS_NOTHING(new RFPort(port1Name, *rf1_));
    TS_ASSERT_THROWS_NOTHING(new RFPort(port2Name, *rf1_));
    TS_ASSERT_THROWS(new RFPort(port1Name, *rf1_), ComponentAlreadyExists);

    const string invalidName = ",,";
    TS_ASSERT_THROWS(new RFPort(invalidName, *rf1_), InvalidName);
}


/**
 * Tests setting the name of the port.
 */
void
RFPortTest::testSetName() {
    RFPort* port1 = new RFPort(port1Name, *rf1_);
    new RFPort(port2Name, *rf1_);
    TS_ASSERT_THROWS_NOTHING(port1->setName(port3Name));
    TS_ASSERT(port1->name() == port3Name);
    TS_ASSERT_THROWS(port1->setName(port2Name), ComponentAlreadyExists);
    TS_ASSERT(port1->name() == port3Name);
}


/**
 * Tests the width() method.
 */
void
RFPortTest::testWidth() {
    RFPort* port1 = new RFPort(port1Name, *rf1_);
    TS_ASSERT(port1->width() == registerWidth);
}


/**
 * Tests attaching and detaching sockets.
 */
void
RFPortTest::testSocketConnections() {

    Socket* socket1 = new Socket(socket1Name);
    RFPort* port1 = new RFPort(port1Name, *rf1_);
    TS_ASSERT_THROWS(port1->attachSocket(*socket1), IllegalRegistration);
    mach_->addSocket(*socket1);
    TS_ASSERT_THROWS_NOTHING(port1->attachSocket(*socket1));
    TS_ASSERT_THROWS(port1->attachSocket(*socket1), ComponentAlreadyExists);

    TS_ASSERT(port1->inputSocket() == NULL);
    TS_ASSERT(port1->outputSocket() == NULL);
    TS_ASSERT(port1->unconnectedSocket(0) == socket1);
    TS_ASSERT(port1->unconnectedSocket(1) == NULL);

    Bus* bus1 = new Bus(bus1Name, 32, 32, Machine::SIGN);
    new Segment(seg1Name, *bus1);
    mach_->addBus(*bus1);
    socket1->attachBus(*bus1->segment(0));
    socket1->setDirection(Socket::INPUT);

    TS_ASSERT(port1->inputSocket() == socket1);
    TS_ASSERT(port1->outputSocket() == NULL);
    TS_ASSERT(port1->unconnectedSocket(0) == NULL);
    TS_ASSERT(port1->unconnectedSocket(1) == NULL);

    Socket* socket2 = new Socket(socket2Name);
    mach_->addSocket(*socket2);
    socket2->attachBus(*bus1->segment(0));
    socket2->setDirection(Socket::INPUT);
    TS_ASSERT_THROWS(port1->attachSocket(*socket2), IllegalConnectivity);
    socket2->setDirection(Socket::OUTPUT);
    TS_ASSERT_THROWS_NOTHING(port1->attachSocket(*socket2));

    TS_ASSERT(port1->inputSocket() == socket1);
    TS_ASSERT(port1->outputSocket() == socket2);
    TS_ASSERT(port1->unconnectedSocket(0) == NULL);
    TS_ASSERT(port1->unconnectedSocket(1) == NULL);

    socket1->detachBus(*bus1);
    socket2->detachBus(*bus1);
    
    TS_ASSERT(port1->unconnectedSocket(0) == socket1 ||
              port1->unconnectedSocket(0) == socket2);
    TS_ASSERT(port1->unconnectedSocket(1) == socket1 ||
              port1->unconnectedSocket(1) == socket2);
    TS_ASSERT(port1->unconnectedSocket(0) != port1->unconnectedSocket(1));
    
    TS_ASSERT_THROWS_NOTHING(port1->detachSocket(*socket1));
    TS_ASSERT_THROWS(port1->detachSocket(*socket1), InstanceNotFound);

    delete port1;
}
    

/**
 * Tests loading the state of the port from an ObjectState instance.
 */
void
RFPortTest::testLoadState() {

    RFPort* port1 = new RFPort(port1Name, *rf1_);
    ObjectState* portState = port1->saveState();
    TS_ASSERT_THROWS(
        new RFPort(portState, *rf1_), ObjectStateLoadingException);
    portState->setAttribute(RFPort::OSKEY_NAME, port2Name);
    TS_ASSERT_THROWS_NOTHING(new RFPort(portState, *rf1_));
    delete portState;

    Socket* socket1 = new Socket(socket1Name);
    mach_->addSocket(*socket1);
    port1->attachSocket(*socket1);
    portState = port1->saveState();
    TS_ASSERT_THROWS_NOTHING(port1->loadState(portState));
    socket1->unsetMachine();
    TS_ASSERT_THROWS(
        port1->loadState(portState), ObjectStateLoadingException);
    socket1->setMachine(*mach_);
    rf1_->unsetMachine();
    TS_ASSERT_THROWS(
        port1->loadState(portState), ObjectStateLoadingException);
    rf1_->setMachine(*mach_);
    TS_ASSERT_THROWS_NOTHING(port1->loadState(portState));

    portState->setAttribute(Port::OSKEY_SECOND_SOCKET, socket1->name());
    TS_ASSERT_THROWS(
        port1->loadState(portState), ObjectStateLoadingException);
    
    Socket* socket2 = new Socket(socket2Name);
    mach_->addSocket(*socket2);
    Bus* bus1 = new Bus(bus1Name, 32, 32, Machine::SIGN);
    new Segment(seg1Name, *bus1);
    mach_->addBus(*bus1);

    socket1->attachBus(*bus1->segment(0));
    socket2->attachBus(*bus1->segment(0));
    socket1->setDirection(Socket::INPUT);
    socket2->setDirection(Socket::OUTPUT);

    portState->setAttribute(Port::OSKEY_SECOND_SOCKET, socket2->name());
    TS_ASSERT_THROWS_NOTHING(port1->loadState(portState));
    TS_ASSERT(port1->inputSocket() == socket1);
    TS_ASSERT(port1->outputSocket() == socket2);
    port1->detachSocket(*socket2);
    socket2->setDirection(Socket::INPUT);
    TS_ASSERT_THROWS(
        port1->loadState(portState), ObjectStateLoadingException);
    
    delete portState;
}

#endif
