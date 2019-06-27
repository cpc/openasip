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
 * @file BridgeTest.hh 
 * A test suite for Bridge.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 10 Jun 2004 by vpj, tr, am, ll
 * @note rating: red
 */

#ifndef BridgeTest_HH
#define BridgeTest_HH

#include <string>

#include <TestSuite.h>
#include "Bridge.hh"
#include "Bus.hh"
#include "Machine.hh"
#include "ADFSerializer.hh"
#include "FileSystem.hh"
#include "Conversion.hh"
#include "ObjectState.hh"

using std::string;
using namespace TTAMachine;

const string bus1Name = "bus1";
const string bus2Name = "bus2";
const string bus3Name = "bus3";
const string bus4Name = "bus4";

class BridgeTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();
    void testSetName();
    void testLoadState();
    void testObjectStateLoadingErrors();

private:
    Machine* mach_;
};


/**
 * Called before each test.
 */
void
BridgeTest::setUp() {
    mach_ = new Machine();
    Bus* bus1 = new Bus(bus1Name, 16, 16, Machine::SIGN);
    Bus* bus2 = new Bus(bus2Name, 16, 16, Machine::SIGN);
    Bus* bus3 = new Bus(bus3Name, 16, 16, Machine::SIGN);
    Bus* bus4 = new Bus(bus4Name, 16, 16, Machine::SIGN);
    mach_->addBus(*bus1);
    mach_->addBus(*bus2);
    mach_->addBus(*bus3);
    mach_->addBus(*bus4);
}


/**
 * Called after each test.
 */
void
BridgeTest::tearDown() {
    delete mach_;
}


/**
 * Tests the creation of bridge.
 */
void
BridgeTest::testCreation() {

    Machine::BusNavigator busNav = mach_->busNavigator();
    Bus* bus1 = busNav.item(bus1Name);
    Bus* bus2 = busNav.item(bus2Name);
    Bus* bus3 = busNav.item(bus3Name);
    Bus* bus4 = busNav.item(bus4Name);

    Bridge* bridge1 = new Bridge("bridge1", *bus1, *bus2);
    Bridge* bridge2 = new Bridge("bridge2", *bus3, *bus4);
    Bridge* bridge3 = new Bridge("bridge3", *bus2, *bus3);
    Bridge* bridge4 = new Bridge("bridge4", *bus3, *bus2);

    if (bus1->hasNextBus()) {
        TS_ASSERT(bus1->nextBus() == bus2);
        TS_ASSERT(bus2->nextBus() == bus3);
        TS_ASSERT(bus3->nextBus() == bus4);
        TS_ASSERT(bus4->previousBus() == bus3);
        TS_ASSERT(bus3->previousBus() == bus2);
        TS_ASSERT(bus2->previousBus() == bus1);
    } else {
        TS_ASSERT(bus1->previousBus() == bus2);
        TS_ASSERT(bus2->previousBus() == bus3);
        TS_ASSERT(bus3->previousBus() == bus4);
        TS_ASSERT(bus4->nextBus() == bus3);
        TS_ASSERT(bus3->nextBus() == bus2);
        TS_ASSERT(bus2->nextBus() == bus1);
    }

    delete bridge1;
    delete bridge2;
    delete bridge3;
    delete bridge4;
    
    // test another case
    bridge1 = new Bridge("bridge1", *bus1, *bus2);
    bridge2 = new Bridge("bridge2", *bus3, *bus1);

    if (bus3->hasNextBus()) {
        TS_ASSERT(bus3->nextBus() == bus1);
        TS_ASSERT(bus1->nextBus() == bus2);
        TS_ASSERT(bus2->previousBus() == bus1);
        TS_ASSERT(bus1->previousBus() == bus3);
    } else {
        TS_ASSERT(bus3->previousBus() == bus1);
        TS_ASSERT(bus1->previousBus() == bus2);
        TS_ASSERT(bus2->nextBus() == bus1);
        TS_ASSERT(bus1->nextBus() == bus3);
    }
    
    TS_ASSERT_THROWS(
        new Bridge("bridge3", *bus2, *bus3), IllegalConnectivity);
    mach_->removeBus(*bus4);
    TS_ASSERT_THROWS(
        new Bridge("bridge3", *bus2, *bus4), IllegalRegistration);
    TS_ASSERT_THROWS(new Bridge("bridge3", *bus2, *bus2), IllegalParameters);
    mach_->addBus(*bus4);
    TS_ASSERT_THROWS(
        new Bridge("bridge2", *bus2, *bus4), ComponentAlreadyExists);

    delete bridge1;
    delete bridge2;

    // test third case
    bridge1 = new Bridge("bridge1", *bus1, *bus2);
    bridge2 = new Bridge("bridge2", *bus4, *bus3);
    bridge3 = new Bridge("bridge3", *bus2, *bus3);
    
    if (bus1->hasNextBus()) {
        TS_ASSERT(bus1->nextBus() == bus2);
        TS_ASSERT(bus2->nextBus() == bus3);
        TS_ASSERT(bus3->nextBus() == bus4);
    } else {
        TS_ASSERT(bus1->previousBus() == bus2);
        TS_ASSERT(bus2->previousBus() == bus3);
        TS_ASSERT(bus3->previousBus() == bus4);
    }

}


/**
 * Tests setting the name of the bridge.
 */
void
BridgeTest::testSetName() {

    const string bridge1Name = "bridge1";
    const string bridge2Name = "bridge2";
    const string bridge3Name = "bridge3";

    Machine::BusNavigator busNav = mach_->busNavigator();
    Bus* bus1 = busNav.item(bus1Name);
    Bus* bus2 = busNav.item(bus2Name);
    Bus* bus3 = busNav.item(bus3Name);
    
    Bridge* bridge1 = new Bridge(bridge1Name, *bus1, *bus2);
    new Bridge(bridge2Name, *bus2, *bus3);

    bridge1->setName(bridge3Name);
    TS_ASSERT(bridge1->name() == bridge3Name);
    TS_ASSERT_THROWS(bridge1->setName(bridge2Name), ComponentAlreadyExists);
}


/**
 * Tests loading the state from ObjectState instance.
 */
void
BridgeTest::testLoadState() {
    
    const string bridge1Name = "bridge1";
    const string bridge2Name = "bridge2";

    ObjectState* fooState = new ObjectState("foo");
    TS_ASSERT_THROWS(
        new Bridge(fooState, *mach_), ObjectStateLoadingException);
    delete fooState;
    
    Machine::BusNavigator busNav = mach_->busNavigator();
    Bus* bus1 = busNav.item(bus1Name);
    Bus* bus2 = busNav.item(bus2Name);
    new Bridge(bridge1Name, *bus1, *bus2);
    
    ObjectState* bridgeState = new ObjectState(Bridge::OSNAME_BRIDGE);
    TS_ASSERT_THROWS(
        new Bridge(bridgeState, *mach_), ObjectStateLoadingException);
    bridgeState->setAttribute(Component::OSKEY_NAME, bridge1Name);
    TS_ASSERT_THROWS(new Bridge(bridgeState, *mach_), 
                     ObjectStateLoadingException);
    bridgeState->setAttribute(Component::OSKEY_NAME, bridge2Name);
    Bridge* bridge2 = new Bridge(bridgeState, *mach_);
    TS_ASSERT_THROWS(
        bridge2->loadState(bridgeState), ObjectStateLoadingException);
    
    bridgeState->setAttribute(Bridge::OSKEY_SOURCE_BUS, "foo");
    bridgeState->setAttribute(Bridge::OSKEY_DESTINATION_BUS, bus1Name);
    TS_ASSERT_THROWS(
        bridge2->loadState(bridgeState), ObjectStateLoadingException);

    bridgeState->setAttribute(Bridge::OSKEY_SOURCE_BUS, bus1Name);
    bridgeState->setAttribute(Bridge::OSKEY_DESTINATION_BUS, bus2Name);
    TS_ASSERT_THROWS(
        bridge2->loadState(bridgeState), ObjectStateLoadingException);
    
    bridgeState->setAttribute(Bridge::OSKEY_SOURCE_BUS, bus2Name);
    bridgeState->setAttribute(Bridge::OSKEY_DESTINATION_BUS, bus1Name);
    bridge2->loadState(bridgeState);
    delete bridgeState;
}


/**
 * Tests loading the state of bridge from erroneous mdf files.
 */
void
BridgeTest::testObjectStateLoadingErrors() {

    const string DIR_SEP = FileSystem::DIRECTORY_SEPARATOR;
    const string SAME_BUS_NAME = "." + DIR_SEP + "data" + DIR_SEP + 
        "SameBusName.mdf";
    const string MULTIPLE_SOURCE_BUS = "." + DIR_SEP + "data" + DIR_SEP +
        "MultipleSourceBus.mdf";
    const string MULTIPLE_DESTINATION_BUS = "." + DIR_SEP + "data" + 
        DIR_SEP + "MultipleDestinationBus.mdf";
    const string CYCLIC_CHAIN = "." + DIR_SEP + "data" + DIR_SEP + 
        "CyclicChain.mdf";
    const string VALID = "." + DIR_SEP + "data" + DIR_SEP + "Valid.mdf";

    ADFSerializer serializer;
    Machine mach;

    serializer.setSourceFile(SAME_BUS_NAME);
    ObjectState* machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(MULTIPLE_SOURCE_BUS);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(MULTIPLE_DESTINATION_BUS);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(CYCLIC_CHAIN);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(VALID);
    machState = serializer.readState();
    TS_ASSERT_THROWS_NOTHING(mach.loadState(machState));
    delete machState;
}
      

#endif
