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
 * @file BusTest.hh 
 *
 * A test suite for Bus.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef BusTest_HH
#define BusTest_HH

#include <string>

#include <TestSuite.h>

#include "Bus.hh"
#include "Machine.hh"
#include "Segment.hh"
#include "ADFSerializer.hh"
#include "FileSystem.hh"
#include "ObjectState.hh"

using std::string;
using namespace TTAMachine;

class BusTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testInvalidInitValues();

    void testSetName();
    void testSetWidth();
    void testSetImmediateWidth();

    void testAddSegment();
    void testReturningSegment();
    void testSaveAndLoadState();
    void testObjectStateLoadingErrors();

};


/**
 * Called before each test.
 */
void
BusTest::setUp() {
}


/**
 * Called after each test.
 */
void
BusTest::tearDown() {
}


/**
 * Tests creating a bus with invalid initial values.
 */
void
BusTest::testInvalidInitValues() {

    Bus* bus = NULL;

    TS_ASSERT_THROWS_ANYTHING(bus = new Bus("test", -1, 1, Machine::SIGN));
    delete bus;
    bus = NULL;

    TS_ASSERT_THROWS_ANYTHING(bus = new Bus("test", 1, -1, Machine::SIGN));
    delete bus;
    bus = NULL;
}


/**
 * Tests setting the name of the bus.
 */
void
BusTest::testSetName() {

    string name = "name";
    string newName = "newName";
    Bus* bus = NULL;

    bus = new Bus(name, 1, 1, Machine::SIGN);
    TS_ASSERT_EQUALS(bus->name(), name);

    bus->setName(newName);
    TS_ASSERT_EQUALS(bus->name(), newName);

    Machine* mach = new Machine();
    mach->addBus(*bus);
    Bus* bus2 = new Bus(name, 1, 1, Machine::SIGN);
    mach->addBus(*bus2);
    TS_ASSERT_THROWS_ANYTHING(bus->setName(name));

    mach->removeBus(*bus);
 
    delete bus;
    bus = NULL;
    delete bus2;
    bus = NULL;
    delete mach;
    mach = NULL;
}


/**
 * Tests setting the width of the bus.
 */
void
BusTest::testSetWidth() {

    Bus* bus = new Bus("name", 1, 1, Machine::SIGN);

    bus->setWidth(32);
    TS_ASSERT_EQUALS(bus->width(), 32);

    TS_ASSERT_THROWS(bus->setWidth(-1), OutOfRange);
    
    Machine* mach = new Machine();
    mach->addBus(*bus);
    delete mach;
}


/**
 * Tests setting the immediate width of the bus.
 */
void
BusTest::testSetImmediateWidth() {

    Bus* bus = new Bus("name", 32, 1, Machine::SIGN);

    bus->setImmediateWidth(32);
    TS_ASSERT_EQUALS(bus->immediateWidth(), 32);

    TS_ASSERT_THROWS(bus->setImmediateWidth(33), OutOfRange);
    TS_ASSERT_THROWS(bus->setImmediateWidth(-1), OutOfRange);

    delete bus;
    bus = NULL;
}


/**
 * Tests the functionality of adding a segment.
 */
void
BusTest::testAddSegment() {
    
    const string busName = "testbus";
    const string segmentName = "testsegment";
    Bus* bus = new Bus("testbus", 16, 16, Machine::SIGN);
    Segment* segment = new Segment(segmentName, *bus);
    
    TS_ASSERT(bus->hasSegment(segmentName) == true);
    TS_ASSERT(bus->segmentCount() == 1);
    
    delete segment;
    delete bus;
}


/**
 * Tests the functionality of returning segments in correct order.
 */
void
BusTest::testReturningSegment() {
    
    Bus* bus = new Bus("bus", 16, 16, Machine::SIGN);
    Segment* seg1 = new Segment("seg1", *bus);
    Segment* seg2 = new Segment("seg2", *bus);
    Segment* seg3 = new Segment("seg3", *bus);
    Segment* seg4 = new Segment("seg4", *bus);
    
    TS_ASSERT(bus->segment(0) == seg1);
    TS_ASSERT(bus->segment(1) == seg2);
    TS_ASSERT(bus->segment(2) == seg3);
    TS_ASSERT(bus->segment(3) == seg4);

    seg4->moveBefore(*seg2);
    TS_ASSERT(bus->segment(0) == seg1);
    TS_ASSERT(bus->segment(1) == seg4);
    TS_ASSERT(bus->segment(2) == seg2);
    TS_ASSERT(bus->segment(3) == seg3);

    TS_ASSERT_THROWS_ANYTHING(bus->segment(-1));
    TS_ASSERT_THROWS_ANYTHING(bus->segment(666));
    TS_ASSERT_THROWS_ANYTHING(bus->segment("invalid"));

    delete bus;
}
    

/**
 * Tests the functionality of saving and loading the bus state.
 */
void
BusTest::testSaveAndLoadState() {

    const string busName = "bus";
    const string invalidName = "invalid name";
    const int busWidth = 32;
    const int immWidth = 16;
    const Machine::Extension extension = Machine::SIGN;
    const string seg1Name = "seg1";

    Machine* mach = new Machine();
    Bus* bus = new Bus(busName, busWidth, immWidth, extension);
    new Segment(seg1Name, *bus);
    mach->addBus(*bus);

    ObjectState* busState = bus->saveState();
    
    mach->removeBus(*bus);
    delete bus;
    Bus* newBus = new Bus("l", 2, 2, Machine::SIGN);
    mach->addBus(*newBus);
    newBus->loadState(busState);

    TS_ASSERT(newBus->name() == busName);
    TS_ASSERT(newBus->width() == busWidth);
    TS_ASSERT(newBus->immediateWidth() == immWidth);
    TS_ASSERT(newBus->signExtends());
    TS_ASSERT(newBus->hasSegment(seg1Name));

    busState->setAttribute(Component::OSKEY_NAME, invalidName);
    TS_ASSERT_THROWS(
        newBus->loadState(busState), ObjectStateLoadingException);
     
    delete busState;
    delete mach;
}
    

/**
 * Tests the handling of errors in xml file that cannot be handled by
 * schema validator.
 */
void
BusTest::testObjectStateLoadingErrors() {

    const string DIR_SEP = FileSystem::DIRECTORY_SEPARATOR;
    const string TWO_SEGMENTS_WITH_SAME_NAME = "." + DIR_SEP + "data" +
        DIR_SEP + "SameNameSegments.mdf";
    const string NO_LAST_SEGMENT = "." + DIR_SEP + "data" + DIR_SEP + 
        "NoLastSegment.mdf";
    const string UNKNOWN_SEGMENT = "." + DIR_SEP + "data" + DIR_SEP +
        "UnknownSegment.mdf";
    const string TWO_SOURCE_SEGMENTS = "." + DIR_SEP + "data" + DIR_SEP +
        "TwoSourceSegments.mdf";
    const string TWO_UNCONDITIONAL_GUARDS = "." + DIR_SEP + "data" + 
        DIR_SEP + "TwoUnconditionalGuards.mdf";
    const string INVALID_REGISTER_IN_GUARD = "." + DIR_SEP + "data" +
        DIR_SEP + "InvalidRegisterInGuard.mdf";
    const string INVALID_PORT_IN_GUARD = "." + DIR_SEP + "data" + DIR_SEP +
        "InvalidPortInGuard.mdf";
    const string UNKNOWN_ADDRESS_SPACE = "." + DIR_SEP + "data" + DIR_SEP +
        "UnknownAddressSpace.adf";
    const string VALID = "." + DIR_SEP + "data" + DIR_SEP + "Valid.mdf";

    ADFSerializer serializer;
    Machine mach;

    serializer.setSourceFile(TWO_SEGMENTS_WITH_SAME_NAME);
    ObjectState* machState = serializer.readState();
    TS_ASSERT_THROWS(
        mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(NO_LAST_SEGMENT);
    machState = serializer.readState();
    TS_ASSERT_THROWS(
        mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(UNKNOWN_SEGMENT); 
    machState = serializer.readState();
    TS_ASSERT_THROWS(
        mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(TWO_SOURCE_SEGMENTS);
    machState = serializer.readState();
    TS_ASSERT_THROWS(
        mach.loadState(machState), ObjectStateLoadingException);
    delete machState;
    
    serializer.setSourceFile(TWO_UNCONDITIONAL_GUARDS);
    machState = serializer.readState();
    TS_ASSERT_THROWS(
        mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(INVALID_REGISTER_IN_GUARD);
    machState = serializer.readState();
    TS_ASSERT_THROWS(
        mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(INVALID_PORT_IN_GUARD);
    machState = serializer.readState();
    TS_ASSERT_THROWS(
        mach.loadState(machState), ObjectStateLoadingException);
    delete machState;    

    serializer.setSourceFile(UNKNOWN_ADDRESS_SPACE);
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
