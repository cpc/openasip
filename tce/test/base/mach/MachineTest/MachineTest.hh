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
 * @file MachineTest.hh
 *
 * A test suite for Machine.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 */

#ifndef MachineTest_HH
#define MachineTest_HH

#include <string>
#include <climits>

#include <TestSuite.h>
#include "Machine.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "Socket.hh"
#include "FunctionUnit.hh"
#include "ImmediateUnit.hh"
#include "RegisterFile.hh"
#include "Bridge.hh"
#include "AddressSpace.hh"
#include "Port.hh"
#include "FUPort.hh"
#include "ControlUnit.hh"
#include "InstructionTemplate.hh"
#include "Exception.hh"

using std::string;
using namespace TTAMachine;

/**
 * Test suite for testing Machine class.
 */
class MachineTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testAddAndRemoveBus();
    void testAddAndRemoveSocket();
    void testAddAndRemoveUnit();
    void testAddAndRemoveBridge();
    void testAddAndDeleteAddressSpace();
    void testAddingFUAndGCUOfSameName();
    void testSaveAndLoadState();

private:
    Machine* mach_;
};


/**
 * Called before each test.
 */
void
MachineTest::setUp() {
    mach_ = new Machine();
}


/**
 * Called after each test.
 */
void
MachineTest::tearDown() {
    if (mach_ != NULL) {
        delete mach_;
        mach_ = NULL;
    }
}


/**
 * Tests adding and removing a bus from machine.
 */
void
MachineTest::testAddAndRemoveBus() {

    const string bus1Name = "bus1";
    const string bus2Name = "bus2";

    Bus* bus1 = new Bus(bus1Name, 16, 16, Machine::SIGN);
    Bus* bus2 = new Bus(bus2Name, 16, 16, Machine::ZERO);

    mach_->addBus(*bus1);
    mach_->addBus(*bus2);

    const string bridgeName = "bridge";
    Bridge* bridge = new Bridge(bridgeName, *bus1, *bus2);
    Machine::BridgeNavigator bridgeNav = mach_->bridgeNavigator();
    TS_ASSERT(bridge->machine() == mach_);
    TS_ASSERT(bridgeNav.item(bridgeName) == bridge);

    Machine::BusNavigator navigator = mach_->busNavigator();
    TS_ASSERT_EQUALS(navigator.item(bus1Name), bus1);
    TS_ASSERT_EQUALS(bus1->machine(), mach_);

    mach_->removeBus(*bus1);

    TS_ASSERT(bus1->machine() == NULL);
    TS_ASSERT(!navigator.hasItem(bus1Name));
    TS_ASSERT(!bridgeNav.hasItem(bridgeName));

    delete bus1;
    delete bus2;
}


/**
 * Tests adding and removing a socket from machine.
 */
void
MachineTest::testAddAndRemoveSocket() {

    const string socket1Name = "socket1";

    Socket socket1(socket1Name);

    mach_->addSocket(socket1);

    Machine::SocketNavigator navigator = mach_->socketNavigator();
    TS_ASSERT_EQUALS(navigator.item(socket1Name), &socket1);
    TS_ASSERT_EQUALS(socket1.machine(), mach_);

    mach_->removeSocket(socket1);

    TS_ASSERT(socket1.machine() == NULL);
    TS_ASSERT(!navigator.hasItem(socket1Name));
}


/**
 * Tests adding and removing an unit from machine.
 */
void
MachineTest::testAddAndRemoveUnit() {

    const string fuName = "FU";
    const string iuName = "IU";
    const string rfName = "RF";

    FunctionUnit fUnit1(fuName);
    RegisterFile rFile1(rfName, 10, 16, 1, 1, 0, RegisterFile::NORMAL);
    ImmediateUnit iUnit1(iuName, 10, 16, 1, 0, Machine::SIGN);

    mach_->addUnit(fUnit1);
    mach_->addUnit(iUnit1);
    mach_->addUnit(rFile1);

    Machine::FunctionUnitNavigator fuNavigator =
        mach_->functionUnitNavigator();
    TS_ASSERT_EQUALS(fuNavigator.item(fuName), &fUnit1);
    TS_ASSERT_EQUALS(fUnit1.machine(), mach_);

     Machine::RegisterFileNavigator rfNavigator =
         mach_->registerFileNavigator();
     TS_ASSERT_EQUALS(rfNavigator.item(rfName), &rFile1);
     TS_ASSERT_EQUALS(rFile1.machine(), mach_);

     Machine::ImmediateUnitNavigator iuNavigator =
         mach_->immediateUnitNavigator();
     TS_ASSERT_EQUALS(iuNavigator.item(iuName), &iUnit1);
     TS_ASSERT_EQUALS(iUnit1.machine(), mach_);

     mach_->removeUnit(fUnit1);

     TS_ASSERT(fUnit1.machine() == NULL);
     TS_ASSERT(!fuNavigator.hasItem(fuName));

     mach_->removeUnit(iUnit1);

     TS_ASSERT(iUnit1.machine() == NULL);
     TS_ASSERT(!iuNavigator.hasItem(iuName));

     mach_->removeUnit(rFile1);

     TS_ASSERT(rFile1.machine() == NULL);
     TS_ASSERT(!rfNavigator.hasItem(rfName));
}


/**
 * Tests adding and removing a bridge from machine.
 */
void
MachineTest::testAddAndRemoveBridge() {

    const string bridgeName = "Bridge";
    const string srcBusName = "srcBus";
    const string dstBusName = "dstBus";

    Bus* srcBus = new Bus(srcBusName, 16, 16, Machine::ZERO);
    Bus* dstBus = new Bus(dstBusName, 16, 16, Machine::ZERO);

    mach_->addBus(*srcBus);
    mach_->addBus(*dstBus);

    Bridge* bridge = new Bridge(bridgeName, *srcBus, *dstBus);

    Machine::BridgeNavigator navigator = mach_->bridgeNavigator();
    TS_ASSERT(bridge->machine() == mach_);
    TS_ASSERT(navigator.item(bridgeName) == bridge);

    mach_->deleteBridge(*bridge);
    TS_ASSERT(!navigator.hasItem(bridgeName));

    delete srcBus;
    delete dstBus;
}


/**
 * Tests adding and deleting an address space.
 */
void
MachineTest::testAddAndDeleteAddressSpace() {

    const string as1Name = "as1";
    const string as2Name = "as2";

    AddressSpace* as1 = new AddressSpace(as1Name, 16, 1, 100, *mach_);
    AddressSpace* as2 = new AddressSpace(as2Name, 16, 101, 200, *mach_);

    Machine::AddressSpaceNavigator navigator =
        mach_->addressSpaceNavigator();

    TS_ASSERT(navigator.item(as1Name) == as1);
    TS_ASSERT(navigator.item(as2Name) == as2);
    TS_ASSERT(as1->machine() == mach_);
    TS_ASSERT(as2->machine() == mach_);

    mach_->deleteAddressSpace(*as1);
    TS_ASSERT(!navigator.hasItem(as1Name));

    delete as2;
    TS_ASSERT(!navigator.hasItem(as2Name));
}


/**
 * Tests adding a GCU and an FU of the same name.
 */
void
MachineTest::testAddingFUAndGCUOfSameName() {

    const string fuName = "fu";
    const string gcuName = "gcu";

    Machine* mach = new Machine();
    ControlUnit* gcu = new ControlUnit(fuName, 2, 1);
    mach->setGlobalControl(*gcu);

    FunctionUnit* fu = new FunctionUnit(fuName);
    TS_ASSERT_THROWS(mach->addFunctionUnit(*fu), ComponentAlreadyExists);

    gcu->setName(gcuName);
    mach->addFunctionUnit(*fu);
    TS_ASSERT_THROWS(fu->setName(gcuName), ComponentAlreadyExists);
    TS_ASSERT_THROWS(gcu->setName(fuName), ComponentAlreadyExists);

    mach->unsetGlobalControl();
    gcu->setName(fuName);
    TS_ASSERT_THROWS(mach->setGlobalControl(*gcu), ComponentAlreadyExists);

    delete gcu;
    delete mach;
}
    
    

/**
 * Tests saving and loading machine state from ObjectState tree.
 */
void
MachineTest::testSaveAndLoadState() {

    const string bus1Name = "bus1";
    const string bus2Name = "bus2";
    const string bus3Name = "bus3";
    const string b1s1Name = "b1s1";
    const string b1s2Name = "b1s2";
    const string b1s3Name = "b1s3";
    const string b2s1Name = "b2s1";
    const string b2s2Name = "b2s2";
    const string b3s1Name = "b3s1";
    const string socket1Name = "socket1";
    const string socket2Name = "socket2";
    const string bridge1Name = "bridge1";
    const string asName = "as";
    const string rfName = "regFile";
    const string rfPort1 = "rfPort1";
    const string fuName = "fu";
    const string fuPortName = "fuPort";
    const string iuName = "iu";
    const string cuName = "cu";
    const string iTempName = "itemp";

    // create buses
    Bus* bus1 = new Bus(bus1Name, 16, 16, Machine::SIGN);
    Bus* bus2 = new Bus(bus2Name, 32, 16, Machine::ZERO);
    Bus* bus3 = new Bus(bus3Name, 32, 16, Machine::SIGN);

    Segment* bus1seg1 = new Segment(b1s1Name, *bus1);
    new Segment(b1s2Name, *bus1);
    new Segment(b1s3Name, *bus1);

    new Segment(b2s1Name, *bus2);
    Segment* bus2seg2 = new Segment(b2s2Name, *bus2);

    new Segment(b3s1Name, *bus3);

    mach_->addBus(*bus1);
    mach_->addBus(*bus2);
    mach_->addBus(*bus3);

    // create sockets
    Socket* socket1 = new Socket(socket1Name);
    mach_->addSocket(*socket1);

    socket1->attachBus(*bus1seg1);
    socket1->attachBus(*bus2seg2);

    // create bridges
    new Bridge(bridge1Name, *bus2, *bus3);

    // create address space
    new AddressSpace(asName, 32, 0, UINT_MAX, *mach_);

    // create register file
    RegisterFile* regFile = new RegisterFile(
        rfName, 30, 16, 1, 1, 0, RegisterFile::NORMAL);
    Port* rfPort = new RFPort(rfPort1, *regFile);
    mach_->addUnit(*regFile);
    rfPort->attachSocket(*socket1);

    // create function unit
    FunctionUnit* fu = new FunctionUnit(fuName);
    FUPort* fuPort = new FUPort(fuPortName, 32, *fu, false, false);
    mach_->addUnit(*fu);
    fuPort->attachSocket(*socket1);

    // create immediate unit
    ImmediateUnit* iu = new ImmediateUnit(iuName, 30, 32, 1, 0, Machine::ZERO);
    mach_->addUnit(*iu);

    // create control unit
    ControlUnit* cu = new ControlUnit(cuName, 5, 1);
    mach_->setGlobalControl(*cu);

    // create instruction template
    InstructionTemplate* iTemp = new InstructionTemplate(iTempName, *mach_);
    iTemp->addSlot(bus1->name(), 16, *iu);

    // save state
    ObjectState* machState = mach_->saveState();

    // load state
    Machine* loadedMach = new Machine();
    loadedMach->loadState(machState);

    // check busses
    Machine::BusNavigator busNav = loadedMach->busNavigator();
    Bus* lBus1 = busNav.item(bus1Name);
    Bus* lBus2 = busNav.item(bus2Name);
    Bus* lBus3 = busNav.item(bus3Name);
    Segment* lBus1Seg1 = lBus1->segment(0);
    TS_ASSERT(lBus1Seg1->name() == b1s1Name);
    Segment* lBus1Seg2 = lBus1Seg1->destinationSegment();
    TS_ASSERT(lBus1Seg2->name() == b1s2Name);
    Segment* lBus1Seg3 = lBus1Seg2->destinationSegment();
    TS_ASSERT(lBus1Seg3->name() == b1s3Name);

    // check sockets
    Machine::SocketNavigator socketNav = loadedMach->socketNavigator();
    Socket* lSocket1 = socketNav.item(socket1Name);

    // check socket-bus connections
    lBus1Seg1 = lBus1->segment(b1s1Name);
    TS_ASSERT(lBus1Seg1->isConnectedTo(*lSocket1));
    Segment* lBus2Seg2 = lBus2->segment(b2s2Name);
    TS_ASSERT(lBus2Seg2->isConnectedTo(*lSocket1));

    // check bridges
    Machine::BridgeNavigator bridgeNav = loadedMach->bridgeNavigator();
    Bridge* lBridge1 = bridgeNav.item(bridge1Name);
    TS_ASSERT(lBridge1->sourceBus() == lBus2);
    TS_ASSERT(lBridge1->destinationBus() == lBus3);

    // check address space
    Machine::AddressSpaceNavigator asNav =
        loadedMach->addressSpaceNavigator();
    TS_ASSERT(asNav.hasItem(asName));
    AddressSpace* lAs = asNav.item(asName);
    TS_ASSERT(lAs->start() == 0);
    TS_ASSERT(lAs->end() == UINT_MAX);

    // check register file
    Machine::RegisterFileNavigator regFileNav =
        loadedMach->registerFileNavigator();
    RegisterFile* rf = regFileNav.item(rfName);
    rfPort = rf->port(rfPort1);
    TS_ASSERT(rfPort->inputSocket() == lSocket1);

    // check function unit
    Machine::FunctionUnitNavigator fuNav =
        loadedMach->functionUnitNavigator();
    FunctionUnit* lFU = fuNav.item(fuName);
    fuPort = lFU->operationPort(fuPortName);
    TS_ASSERT(fuPort->inputSocket() == lSocket1);

    // check immediate unit
    Machine::ImmediateUnitNavigator iuNav =
        loadedMach->immediateUnitNavigator();
    TS_ASSERT(iuNav.hasItem(iuName));

    // check control unit
    ControlUnit* lCU = loadedMach->controlUnit();
    TS_ASSERT(lCU->name() == cuName);
    TS_ASSERT(lCU->globalGuardLatency() == 1);

    // check instruction template
    Machine::InstructionTemplateNavigator iTempNav =
        loadedMach->instructionTemplateNavigator();
    InstructionTemplate* lIT = iTempNav.item(iTempName);
    TS_ASSERT(lIT->usesSlot(lBus1->name()));

    delete machState;
    delete loadedMach;
}

#endif
