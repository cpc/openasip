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
 * @file SocketTest.hh 
 *
 * A test suite for Socket class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SOCKET_TEST_HH
#define TTA_SOCKET_TEST_HH

#include <string>

#include <TestSuite.h>

#include "Machine.hh"
#include "Socket.hh"
#include "Bus.hh"
#include "Connection.hh"
#include "Segment.hh"
#include "FUPort.hh"
#include "ADFSerializer.hh"
#include "ObjectState.hh"
#include "FileSystem.hh"

using std::string;
using namespace TTAMachine;

// constants
const string SOCKET_NAME_1 = "socketName1";
const string SOCKET_NAME_2 = "socketName2";
const string BUS_NAME = "busName";
const string SEGMENT_NAME = "segmentName";


class SocketTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testSetName();
    void testSetDirection();
    void testAttachAndDetachBus();

    void testReturnPort();
    void testReturnSegment();

    void testLoadState();
    void testObjectStateLoadingErrors();

private:
    /// A socket used in tests.
    Socket* socket1_;
    /// Another socket used in tests.
    Socket* socket2_;
    /// A bus used in tests.
    Bus* bus_;
    /// A segment used in tests.
    Segment* segment_;
    /// A machine used in tests.
    Machine* machine_;
};


/**
 * Called before each test.
 *
 * Creates objects needed in tests. Socket 1 is registered to the machine and 
 * attached to the bus (using segment). Socket 2 is created as an independent
 * object.
 */
void
SocketTest::setUp() {
    socket1_ = new Socket(SOCKET_NAME_1);
    socket2_ = new Socket(SOCKET_NAME_2);
    bus_ = new Bus(BUS_NAME, 16, 16, Machine::SIGN);
    segment_ = new Segment(SEGMENT_NAME, *bus_);
    machine_ = new Machine();
    machine_->addSocket(*socket1_);
    machine_->addBus(*bus_);
    socket1_->attachBus(*segment_);
}


/**
 * Called after each test.
 *
 * Deletes objects used in test.
 */
void
SocketTest::tearDown() {
    if (bus_ != NULL) {
        delete bus_;
        bus_ = NULL;
    }
    if (socket1_ != NULL) {
        delete socket1_;
        socket1_ = NULL;
    }
    if (socket2_ != NULL) {
        delete socket2_;
        socket2_ = NULL;
    }
    if (machine_ != NULL) {
        delete machine_;
        machine_ = NULL;
    }
}


/**
 * Tests setting the name.
 */
void
SocketTest::testSetName() {

    TS_ASSERT_EQUALS(socket1_->name(), SOCKET_NAME_1);

    socket1_->setName(SOCKET_NAME_2);
    TS_ASSERT_EQUALS(socket1_->name(), SOCKET_NAME_2);

    socket1_->setName(SOCKET_NAME_1);
    machine_->addSocket(*socket2_);
    TS_ASSERT_THROWS(socket2_->setName(SOCKET_NAME_1), ComponentAlreadyExists);
}


/**
 * Tests setting the direction.
 */
void
SocketTest::testSetDirection() {

    TS_ASSERT_EQUALS(socket2_->direction(), Socket::UNKNOWN);
    TS_ASSERT_THROWS(
        socket2_->setDirection(Socket::INPUT), IllegalConnectivity);

    TS_ASSERT_THROWS(
        socket1_->setDirection(Socket::UNKNOWN), IllegalConnectivity);

    socket1_->setDirection(Socket::INPUT);
    TS_ASSERT_EQUALS(socket1_->direction(), Socket::INPUT);
}



/**
 * Tests attaching and detaching a bus.
 */
void
SocketTest::testAttachAndDetachBus() {

    TS_ASSERT(socket1_->isConnectedTo(*bus_));
    TS_ASSERT(socket1_->isConnectedTo(*segment_));
    TS_ASSERT(bus_->isConnectedTo(*socket1_));
    TS_ASSERT(socket1_->direction() == Socket::INPUT);

    const Connection& CONN_1 = socket1_->connection(*segment_);
    const Connection& CONN_2 = segment_->connection(*socket1_);
    TS_ASSERT(&CONN_1 == &CONN_2);
    TS_ASSERT(CONN_1.socket() == socket1_);
    TS_ASSERT(CONN_1.bus() == segment_);

    socket1_->detachBus(*bus_);
    TS_ASSERT(socket1_->direction() == Socket::UNKNOWN);

    TS_ASSERT(!(bus_->isConnectedTo(*socket1_)));
    TS_ASSERT(!(segment_->isConnectedTo(*socket1_)));
    TS_ASSERT(!(socket1_->isConnectedTo(*bus_)));
    TS_ASSERT(!(socket1_->isConnectedTo(*segment_)));

    FunctionUnit* fu = new FunctionUnit("fu");
    FUPort* port1 = new FUPort("port1", 32, *fu, false, false);
    machine_->addFunctionUnit(*fu);
    Socket* socket2 = new Socket("socket2");
    machine_->addSocket(*socket2);
    socket2->attachBus(*segment_);
    TS_ASSERT(socket2->direction() == Socket::INPUT);

    port1->attachSocket(*socket2);
    port1->attachSocket(*socket1_);
    socket1_->attachBus(*segment_);
    TS_ASSERT(socket1_->direction() == Socket::OUTPUT);

    socket1_->detachBus(*segment_);
    TS_ASSERT_THROWS(socket1_->detachBus(*segment_), InstanceNotFound);
}


/**
 * Tests the returning of a port.
 */
void
SocketTest::testReturnPort() {
    TS_ASSERT_THROWS(socket2_->port(-1), OutOfRange);
}


/**
 * Tests the returning of a segment.
 */
void
SocketTest::testReturnSegment() {
    TS_ASSERT_THROWS(socket2_->segment(-1), OutOfRange);
    TS_ASSERT_EQUALS(socket1_->segmentCount(), 1);
}


/**
 * Tests creating a socket from an ObjectState object.
 */
void
SocketTest::testLoadState() {

    ObjectState* state = bus_->saveState();

    TS_ASSERT_THROWS(
        socket2_->loadState(state), ObjectStateLoadingException);

    delete state;
    state = socket1_->saveState();
    TS_ASSERT_THROWS_NOTHING(socket1_->loadState(state));

    bus_->unsetMachine();
    TS_ASSERT_THROWS(
        socket1_->loadState(state), ObjectStateLoadingException);

    bus_->setMachine(*machine_);
    delete segment_;
    TS_ASSERT_THROWS(
        socket1_->loadState(state), ObjectStateLoadingException);

    segment_ = new Segment(SEGMENT_NAME, *bus_);
    state->setAttribute(Socket::OSKEY_DIRECTION, Socket::OSVALUE_UNKNOWN);
    TS_ASSERT_THROWS(
        socket1_->loadState(state), ObjectStateLoadingException);

    delete state;
}


/**
 * Tests creating socket from an erronous mdf file.
 */
void
SocketTest::testObjectStateLoadingErrors() {

    const string DIR_SEP = FileSystem::DIRECTORY_SEPARATOR;
    const string INVALID_SEGMENT_REFERENCE = "." + DIR_SEP + "data" + 
        DIR_SEP + "InvalidSegmentReference.mdf";
    const string MULTIPLE_SEGMENT = "." + DIR_SEP + "data" + DIR_SEP + 
        "MultipleSegment.mdf";
    const string VALID = "." + DIR_SEP + "data" + DIR_SEP + "Valid.mdf";

    ADFSerializer serializer;
    Machine mach;

    serializer.setSourceFile(INVALID_SEGMENT_REFERENCE);
    ObjectState* machState = serializer.readState();
    TS_ASSERT_THROWS(
        mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(MULTIPLE_SEGMENT);
    machState = serializer.readState();
    TS_ASSERT_THROWS(
        mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(VALID);
    machState = serializer.readState();
    TS_ASSERT_THROWS_NOTHING(mach.loadState(machState));
    delete machState;
}

#endif
