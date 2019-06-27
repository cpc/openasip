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
 * @file RegisterFileTest.hh 
 *
 * A test suite for RegisterFile class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef RegisterFileTest_HH
#define RegisterFileTest_HH

#include <string>

#include <TestSuite.h>
#include "Port.hh"
#include "Segment.hh"
#include "ADFSerializer.hh"
#include "FileSystem.hh"
#include "RegisterFile.hh"
#include "Machine.hh"
#include "ObjectState.hh"

using std::string;
using namespace TTAMachine;

/**
 * Tests the functionality of RegisterFile class.
 */
class RegisterFileTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testSetName();
    void testSetNumberOfRegisters();
    void testManualMaxReadsAndWrites();
    void testAutomaticMaxReadsAndWrites();
    void testSetWidth();
    void testLoadState();
    void testObjectStateLoadingErrors();
};


/**
 * Called before each test.
 */
void
RegisterFileTest::setUp() {
}


/**
 * Called after each test.
 */
void
RegisterFileTest::tearDown() {
}


/**
 * Tests setting the name of the register file.
 */
void
RegisterFileTest::testSetName() {

    const string rf1Name = "rf1";
    const string rf2Name = "rf2";
    const string otherName = "foo";
    
    Machine mach;
    RegisterFile* rf1 = new RegisterFile(
        rf1Name, 30, 32, 1, 1, 0, RegisterFile::NORMAL);
    TS_ASSERT(rf1->name() == rf1Name);
    TS_ASSERT_THROWS_NOTHING(rf1->setName(rf1Name));
    TS_ASSERT_THROWS_NOTHING(rf1->setName(rf2Name));
    TS_ASSERT(rf1->name() == rf2Name);
    rf1->setName(rf1Name);
    
    mach.addRegisterFile(*rf1);

    RegisterFile* rf2 = new RegisterFile(
        rf2Name, 30, 32, 1, 1, 0, RegisterFile::NORMAL);
    mach.addRegisterFile(*rf2);

    TS_ASSERT_THROWS(rf2->setName(rf1Name), ComponentAlreadyExists);
    TS_ASSERT(rf2->name() == rf2Name);
    TS_ASSERT_THROWS_NOTHING(rf2->setName(otherName));
    TS_ASSERT(rf2->name() == otherName);
}


/**
 * Tests setting the number of registers.
 */
void
RegisterFileTest::testSetNumberOfRegisters() {
    RegisterFile rf1("rf1", 30, 32, 1, 1, 0, RegisterFile::NORMAL);
    TS_ASSERT_THROWS(rf1.setNumberOfRegisters(0), OutOfRange);
    TS_ASSERT(rf1.numberOfRegisters() == 30);
    TS_ASSERT_THROWS_NOTHING(rf1.setNumberOfRegisters(20));
    TS_ASSERT(rf1.numberOfRegisters() == 20);
}


/**
 * Tests calculating automatically the number of maxWrites and maxReads.
 */
void
RegisterFileTest::testAutomaticMaxReadsAndWrites() {
    Machine mach;

    // set maxReads and maxWrites to 0
    RegisterFile rf("rf1", 30, 32, 0, 0, 0, RegisterFile::NORMAL);
    mach.addRegisterFile(rf);

    TS_ASSERT(rf.maxReads() == 0);
    TS_ASSERT(rf.maxWrites() == 0);

    Bus bus("bus", 8, 8, Machine::ZERO);
    Segment segment("segment", bus);
    mach.addBus(bus);

    // create input socket
    Socket sock1("sock1");
    mach.addSocket(sock1);
    sock1.attachBus(segment);
    sock1.setDirection(Socket::INPUT);

    // create output socket
    Socket sock2("sock2");
    mach.addSocket(sock2);
    sock2.attachBus(segment);
    sock2.setDirection(Socket::OUTPUT);

    // create port and attach it to socket
    RFPort port1("port1", rf);
    port1.attachSocket(sock1);

    // since the port was attached to input socket, maxWrites should now be 1
    TS_ASSERT(rf.maxReads() == 0);
    TS_ASSERT(rf.maxWrites() == 1);

    // attach another port to output socket
    RFPort port2("port2", rf);
    port2.attachSocket(sock2);

    // maxReads should have increased
    TS_ASSERT(rf.maxReads() == 1);
    TS_ASSERT(rf.maxWrites() == 1);
}

/**
 * Tests setting manually the number of maxWrites and maxReads.
 */
void
RegisterFileTest::testManualMaxReadsAndWrites() {
    Machine mach;

    // set maxReads and maxWrites to 9
    RegisterFile rf("rf1", 30, 32, 9, 9, 0, RegisterFile::NORMAL);
    mach.addRegisterFile(rf);

    TS_ASSERT(rf.maxReads() == 9);
    TS_ASSERT(rf.maxWrites() == 9);

    Bus bus("bus", 8, 8, Machine::ZERO);
    Segment segment("segment", bus);
    mach.addBus(bus);

    // create port in registerfile
    RFPort port1("port1", rf);

    // since the port was not attached, reads and writes should be unaffected
    TS_ASSERT(rf.maxReads() == 9);
    TS_ASSERT(rf.maxWrites() == 9);

    // set maxReads and maxWrites to 1
    rf.setMaxReads(1);
    rf.setMaxWrites(1);

    // verify
    TS_ASSERT(rf.maxReads() == 1);
    TS_ASSERT(rf.maxWrites() == 1);
}


/**
 * Tests setting the width of the register file.
 */
void
RegisterFileTest::testSetWidth() {
    RegisterFile rf1("rf1", 30, 32, 1, 1, 0, RegisterFile::NORMAL);
    TS_ASSERT_THROWS(rf1.setWidth(0), OutOfRange);
    TS_ASSERT(rf1.width() == 32);
    TS_ASSERT_THROWS_NOTHING(rf1.setWidth(16));
    TS_ASSERT(rf1.width() == 16);
}
   

/**
 * Tests saving the state of register file to an ObjectState tree and
 * loading the state from it.
 */
void
RegisterFileTest::testLoadState() {

    const string rf1Name = "rf1";

    RegisterFile rf(rf1Name, 30, 32, 0, 0, 0, RegisterFile::NORMAL);
    ObjectState* rf1State = rf.saveState();

    RegisterFile loaded(rf1State);
    TS_ASSERT(loaded.name() == rf1Name);
    TS_ASSERT(loaded.numberOfRegisters() == 30);
    TS_ASSERT(loaded.width() == 32);
    TS_ASSERT(loaded.maxReads() == 0);
    TS_ASSERT(loaded.maxWrites() == 0);
    TS_ASSERT(loaded.type() == RegisterFile::NORMAL);

    delete rf1State;
    rf.setType(RegisterFile::RESERVED);
    rf1State = rf.saveState();
    TS_ASSERT_THROWS_NOTHING(loaded.loadState(rf1State));
    TS_ASSERT(loaded.type() == RegisterFile::RESERVED);

    delete rf1State;
    rf.setType(RegisterFile::VOLATILE);
    rf1State = rf.saveState();
    TS_ASSERT_THROWS_NOTHING(loaded.loadState(rf1State));
    TS_ASSERT(loaded.type() == RegisterFile::VOLATILE);

    rf1State->setAttribute(RegisterFile::OSKEY_TYPE, "unknownType");
    TS_ASSERT_THROWS(
        loaded.loadState(rf1State), ObjectStateLoadingException);
    delete rf1State;
}
   

/**
 * Tests creating register file from an erronous mdf file.
 */
void
RegisterFileTest::testObjectStateLoadingErrors() {

    const string DIR_SEP = FileSystem::DIRECTORY_SEPARATOR;
    const string PORTS_CONNECTED_TO_SAME_SOCKET = "." + DIR_SEP + "data" +
        DIR_SEP + "PortsConnectedToSameSocket.mdf";
    const string INVALID_SOCKET_NAME = "." + DIR_SEP + "data" + DIR_SEP +
        "InvalidSocketName.mdf";
    const string SAME_SOCKET_DIRECTION = "." + DIR_SEP + "data" + DIR_SEP +
        "SameSocketDirection.mdf";
    const string VALID = "." + DIR_SEP + "data" + DIR_SEP + "Valid.mdf";

    ADFSerializer serializer;
    Machine mach;

    serializer.setSourceFile(PORTS_CONNECTED_TO_SAME_SOCKET);
    ObjectState* machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;
    
    serializer.setSourceFile(INVALID_SOCKET_NAME);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(SAME_SOCKET_DIRECTION);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(VALID);
    machState = serializer.readState();
    TS_ASSERT_THROWS_NOTHING(mach.loadState(machState));
    delete machState;    
}

#endif
