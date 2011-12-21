/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file ControlUnitTest.hh 
 *
 * A test suite for ControlUnit class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef ControlUnitTest_HH
#define ControlUnitTest_HH

#include <string>

#include <TestSuite.h>
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"
#include "ADFSerializer.hh"
#include "FileSystem.hh"
#include "Machine.hh"

using std::string;
using namespace TTAMachine;

/**
 * Tests the functionality of ControlUnit class.
 */
class ControlUnitTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testSetName();
    void testSetDelaySlots();
    void testReturnAddressPort();
    void testObjectStateLoadingErrors();
};


/**
 * Called before each test.
 */
void
ControlUnitTest::setUp() {
}


/**
 * Called after each test.
 */
void
ControlUnitTest::tearDown() {
}


/**
 * Tests setting the name for GCU.
 */
void
ControlUnitTest::testSetName() {
    
    const string fuName = "fu";
    const string gcuName = "gcu";

    Machine* mach = new Machine();
    FunctionUnit* fu = new FunctionUnit(fuName);
    mach->addFunctionUnit(*fu);

    ControlUnit* gcu = new ControlUnit(fuName, 2, 1);
    TS_ASSERT_THROWS(mach->setGlobalControl(*gcu), ComponentAlreadyExists);

    gcu->setName(gcuName);
    mach->setGlobalControl(*gcu);

    TS_ASSERT_THROWS(gcu->setName(fuName), ComponentAlreadyExists);

    delete mach;
}

/**
 * Tests setting the number of delay slots.
 */
void
ControlUnitTest::testSetDelaySlots() {   
    ControlUnit cu("cu", 2, 1);
    TS_ASSERT_THROWS(cu.setDelaySlots(-1), OutOfRange);
    TS_ASSERT(cu.delaySlots() == 2);
    TS_ASSERT_THROWS_NOTHING(cu.setDelaySlots(3));
    TS_ASSERT(cu.delaySlots() == 3);
}


/**
 * Tests the functionality of playing with return address port.
 */
void
ControlUnitTest::testReturnAddressPort() {
    
    ControlUnit* cu = new ControlUnit("cu", 1, 1);
    SpecialRegisterPort* raPort = new SpecialRegisterPort("ra", 32, *cu);
    TS_ASSERT(!cu->hasReturnAddressPort());
    TS_ASSERT_THROWS(cu->returnAddressPort(), InstanceNotFound);
    
    cu->setReturnAddressPort(*raPort);
    TS_ASSERT(cu->hasReturnAddressPort());
    TS_ASSERT(cu->returnAddressPort() == raPort);

    cu->unsetReturnAddressPort();
    TS_ASSERT(!cu->hasReturnAddressPort());

    cu->setReturnAddressPort(*raPort);
    TS_ASSERT(cu->hasReturnAddressPort());
    delete raPort;
    TS_ASSERT(!cu->hasReturnAddressPort());
    
    delete cu;
}
    

/**
 * Tests creating control unit from an erronous mdf file.
 */
void
ControlUnitTest::testObjectStateLoadingErrors() {

    const string DIR_SEP = FileSystem::DIRECTORY_SEPARATOR;
    const string INVALID_SOCKET_IN_SPECIAL_PORT = "." + DIR_SEP + "data" +
        DIR_SEP + "InvalidSocketInSpecialPort.adf";
    const string SAME_DIRECTION_SOCKETS_IN_SPECIAL_PORT = "." + DIR_SEP +
        "data" + DIR_SEP + "SameDirectionSocketsInSpecialPort.adf";
    const string UNKNOWN_ADDRESS_SPACE = "." + DIR_SEP + "data" + DIR_SEP +
        "UnknownAddressSpace.adf";
    const string INVALID_GUARD_LATENCY = "." + DIR_SEP + "data" + DIR_SEP +
        "InvalidGuardLatency.adf";
    const string VALID = "." + DIR_SEP + "data" + DIR_SEP + "Valid.mdf";

    ADFSerializer serializer;
    Machine mach;

    serializer.setSourceFile(INVALID_SOCKET_IN_SPECIAL_PORT);
    ObjectState* machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(SAME_DIRECTION_SOCKETS_IN_SPECIAL_PORT);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(UNKNOWN_ADDRESS_SPACE);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(INVALID_GUARD_LATENCY);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;
    
    serializer.setSourceFile(VALID);
    machState = serializer.readState();
    TS_ASSERT_THROWS_NOTHING(mach.loadState(machState));
    delete machState;    
}

#endif
