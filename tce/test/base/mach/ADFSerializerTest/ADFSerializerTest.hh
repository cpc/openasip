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
 * @file ADFSerializerTest.hh 
 * A test suite for ADFSerializer.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 15 Jun 2004 by pj, vpj, ml, ll
 * @note rating: red
 */

#ifndef ADFSerializerTest_HH
#define ADFSerializerTest_HH

#include <string>
#include <TestSuite.h>

#include "ADFSerializer.hh"
#include "Machine.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "Guard.hh"
#include "Socket.hh"
#include "Bridge.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "FUPort.hh"
#include "SpecialRegisterPort.hh"
#include "RegisterFile.hh"
#include "Port.hh"
#include "ImmediateUnit.hh"
#include "AddressSpace.hh"
#include "ControlUnit.hh"
#include "InstructionTemplate.hh"
#include "FileSystem.hh"

using namespace TTAMachine;
using std::string;

const string bus1Name = "bus1";
const string bus2Name = "bus2";
const string slot1Name = "slot1";
const string seg1Name = "seg1";
const string seg2Name = "seg2";
const string seg3Name = "seg3";
const string br1Name = "br1";
const string socket1Name = "socket1";
const string fu1Name = "fu1";
const string port1Name = "port1";
const string port2Name = "port2";
const string port3Name = "port3";
const string rf1Name = "rf1";
const string iu1Name = "iu1";
const string iu2Name = "iu2";
const string as1Name = "as1";
const string cuName = "cu";
const string it1Name = "it1";
const string it2Name = "it2";
const string op1Name = "op1";
const string res1Name = "res1";
const string controlPortName = "control";
const string raPortName = "ra";

const string DIR_SEP = FileSystem::DIRECTORY_SEPARATOR;
const string FILE_TO_WRITE = "." + DIR_SEP + "data" + DIR_SEP + 
"written.mdf";


/**
 * Tests different functionality of ADFSerializer class.
 */
class ADFSerializerTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    
    void testWriteState();
    void testReadState();

private:
};


/**
 * Called before each test.
 */
void
ADFSerializerTest::setUp() {
}


/**
 * Called after each test.
 */
void
ADFSerializerTest::tearDown() {
}


/**
 * Tests the functionality of writeState function.
 */
void
ADFSerializerTest::testWriteState() {

    Machine* mach = new Machine();

    TS_ASSERT(!mach->triggerInvalidatesResults());
    TS_ASSERT(!mach->alwaysWriteResults());
    TS_ASSERT(!mach->isFUOrdered());
    mach->setTriggerInvalidatesResults(true);
    TS_ASSERT(mach->triggerInvalidatesResults());    
    mach->setFUOrdered(false);
        
    Bus* bus1 = new Bus(bus1Name, 16, 16, Machine::SIGN);
    Bus* bus2 = new Bus(bus2Name, 32, 32, Machine::ZERO);
    new Segment(seg1Name, *bus1);
    new Segment(seg2Name, *bus1);
    new Segment(seg1Name, *bus2);
    new Segment(seg2Name, *bus2);
    new Segment(seg3Name, *bus2);
    mach->addBus(*bus1);
    mach->addBus(*bus2);

    new Bridge(br1Name, *bus1, *bus2);

    new ImmediateSlot(slot1Name, *mach);

    Socket* socket1 = new Socket(socket1Name);
    mach->addSocket(*socket1);
    socket1->attachBus(*bus1->segment(0));
    socket1->setDirection(Socket::OUTPUT);

    FunctionUnit* fu1 = new FunctionUnit(fu1Name);
    TS_ASSERT(fu1->orderNumber() == 0);
    FUPort* fuPort1 = new FUPort(port1Name, 16, *fu1, true, true);
    FUPort* fuPort2 = new FUPort(port2Name, 16, *fu1, false, false, true);    
    FUPort* fuPort3 = new FUPort(port3Name, 16, *fu1, false, false, true);        
    mach->addUnit(*fu1);
    HWOperation* op1 = new HWOperation(op1Name, *fu1);
    op1->bindPort(1, *fuPort1);
    ExecutionPipeline* pLine = op1->pipeline();
    pLine->addResourceUse(res1Name, 0, 5);
    pLine->addPortRead(1, 0, 2);

    fuPort1->attachSocket(*socket1);
    fuPort2->attachSocket(*socket1);
    fuPort3->attachSocket(*socket1);    
    fuPort3->setNoRegister(false);

    RegisterFile* rf1 = new RegisterFile(
        rf1Name, 15, 32, 1, 1, 0, RegisterFile::NORMAL);
    Port* rfPort1 = new RFPort(port1Name, *rf1);
    mach->addUnit(*rf1);
    rfPort1->attachSocket(*socket1);

    ImmediateUnit* iUnit1 = new ImmediateUnit(
        iu1Name, 30, 16, 1, 0, Machine::SIGN);
    new RFPort(port1Name, *iUnit1);
    mach->addImmediateUnit(*iUnit1);
    ImmediateUnit* iUnit2 = new ImmediateUnit(
        iu2Name, 30, 16, 1, 0, Machine::SIGN);
    mach->addImmediateUnit(*iUnit2);

    AddressSpace* as1 = new AddressSpace(as1Name, 16, 0, UINT_MAX, *mach);
    fu1->setAddressSpace(as1);

    ControlUnit* cUnit1 = new ControlUnit(cuName, 3, 1);
    mach->setGlobalControl(*cUnit1);
    new FUPort(controlPortName, 16, *cUnit1, true, true);
    SpecialRegisterPort* raPort = 
        new SpecialRegisterPort(raPortName, 16, *cUnit1);
    cUnit1->setReturnAddressPort(*raPort);

    InstructionTemplate* iTemp = new InstructionTemplate(it1Name, *mach);
    iTemp->addSlot(bus1Name, 16, *iUnit1);
    iTemp->addSlot(bus2Name, 32, *iUnit2);
    iTemp->addSlot(slot1Name, 32, *iUnit2);
    new InstructionTemplate(it2Name, *mach);

    new UnconditionalGuard(false, *bus1);
    new PortGuard(false, *fuPort1, *bus1);
    new RegisterGuard(false, *rf1, 0, *bus1);

    ADFSerializer* serializer = new ADFSerializer();
    serializer->setDestinationFile(FILE_TO_WRITE);
    TS_ASSERT_THROWS_NOTHING(serializer->writeMachine(*mach));

    delete mach;
    delete serializer;
}


/**
 * Tests the functionality of readState function.
 */
void
ADFSerializerTest::testReadState() {

    ADFSerializer* serializer = new ADFSerializer();
    serializer->setSourceFile(FILE_TO_WRITE);

    Machine* mach = serializer->readMachine();

    TS_ASSERT(mach->triggerInvalidatesResults());
    TS_ASSERT(!mach->alwaysWriteResults());
    TS_ASSERT(!mach->isFUOrdered());

    Machine::BusNavigator busNav = mach->busNavigator();
    Bus* bus1 = busNav.item(bus1Name);
    Bus* bus2 = busNav.item(bus2Name);

    TS_ASSERT(bus1->segment(0)->name() == seg1Name);
    TS_ASSERT(bus1->segment(1)->name() == seg2Name);
    TS_ASSERT(bus2->segment(0)->name() == seg1Name);
    TS_ASSERT(bus2->segment(1)->name() == seg2Name);
    TS_ASSERT(bus2->segment(2)->name() == seg3Name);

    TS_ASSERT(bus1->guardCount() == 3);

    TS_ASSERT(mach->immediateSlotNavigator().hasItem(slot1Name));
     
    Socket* socket1 = mach->socketNavigator().item(socket1Name);
    TS_ASSERT(socket1->isConnectedTo(*bus1));
    TS_ASSERT(socket1->direction() == Socket::OUTPUT);
    
    Bridge* bridge1 = mach->bridgeNavigator().item(br1Name);
    TS_ASSERT(bridge1->sourceBus() == bus1);
    TS_ASSERT(bridge1->destinationBus() == bus2);
    
    FunctionUnit* fu1 = mach->functionUnitNavigator().item(fu1Name);
    // When reading FU from file, it always gets unique order ID
    TS_ASSERT(fu1->orderNumber() != 0);
    FUPort* fuPort1 = fu1->operationPort(port1Name);
    TS_ASSERT(fuPort1->width() == 16);
    TS_ASSERT(fuPort1->isTriggering());
    TS_ASSERT(fuPort1->isOpcodeSetting());
    TS_ASSERT(!fuPort1->noRegister());
    FUPort* fuPort2 = fu1->operationPort(port2Name);   
    TS_ASSERT(fuPort2->width() == 16);
    TS_ASSERT(!fuPort2->isTriggering());
    TS_ASSERT(!fuPort2->isOpcodeSetting());
    TS_ASSERT(fuPort2->noRegister());    
    FUPort* fuPort3 = fu1->operationPort(port3Name);   
    TS_ASSERT(fuPort3->width() == 16);
    TS_ASSERT(!fuPort3->isTriggering());
    TS_ASSERT(!fuPort3->isOpcodeSetting());
    TS_ASSERT(!fuPort3->noRegister());    
    
    TS_ASSERT(fu1->hasOperation(op1Name));
    HWOperation* op1 = fu1->operation(op1Name);
    TS_ASSERT(op1->port(1) == fuPort1);
    ExecutionPipeline* pLine = op1->pipeline();
    TS_ASSERT(pLine->isResourceUsed(res1Name, 0));
    TS_ASSERT(!pLine->isResourceUsed(res1Name, 6));
    TS_ASSERT(pLine->isPortRead(*fuPort1, 0));
    
    RegisterFile* rf1 = mach->registerFileNavigator().item(rf1Name);
    TS_ASSERT(rf1->numberOfRegisters() == 15);
    TS_ASSERT(rf1->width() == 32);
    TS_ASSERT(rf1->maxReads() == 1);
    TS_ASSERT(rf1->maxWrites() == 0);
    TS_ASSERT(rf1->isNormal());
    
    ImmediateUnit* iu1 = mach->immediateUnitNavigator().item(iu1Name);
    TS_ASSERT(iu1->numberOfRegisters() == 30);
    TS_ASSERT(iu1->width() == 16);
    TS_ASSERT(iu1->extensionMode() == Machine::SIGN);
    TS_ASSERT(iu1->latency() == 1);
    ImmediateUnit* iu2 = mach->immediateUnitNavigator().item(iu2Name);

    AddressSpace* as1 = mach->addressSpaceNavigator().item(as1Name);
    TS_ASSERT(as1->width() == 16);
    TS_ASSERT(as1->start() == 0);
    TS_ASSERT(as1->end() == UINT_MAX);

    ControlUnit* cu = mach->controlUnit();
    TS_ASSERT(cu->name() == cuName);
    TS_ASSERT(cu->delaySlots() == 3);
    TS_ASSERT(cu->hasPort(controlPortName));
    TS_ASSERT(cu->returnAddressPort()->name() == raPortName);

    InstructionTemplate* it1 = mach->instructionTemplateNavigator().item(
        it1Name);
    TS_ASSERT(it1->usesSlot(bus1Name));
    TS_ASSERT(it1->usesSlot(bus2Name));
    TS_ASSERT(it1->usesSlot(slot1Name));
    TS_ASSERT(it1->numberOfDestinations() == 2);
    TS_ASSERT(it1->isOneOfDestinations(*iu1));
    TS_ASSERT(it1->isOneOfDestinations(*iu2));
    TS_ASSERT(it1->supportedWidth(*iu1) == 16);
    InstructionTemplate* it2 = mach->instructionTemplateNavigator().item(
        it2Name);
    TS_ASSERT(it2->isEmpty());

    TS_ASSERT(bus1->guardCount() == 3);
    for (int i = 0; i < bus1->guardCount(); i++) {
        Guard* guard = bus1->guard(i);
        PortGuard* pGuard = dynamic_cast<PortGuard*>(guard);
        RegisterGuard* rGuard = dynamic_cast<RegisterGuard*>(guard);
        if (pGuard != NULL) {
            TS_ASSERT(pGuard->port()->name() == port1Name);
            TS_ASSERT(pGuard->port()->parentUnit() == fu1);
        } else if (rGuard != NULL) {
            TS_ASSERT(rGuard->registerFile() == rf1);
            TS_ASSERT(rGuard->registerIndex() == 0);
        }
    }

    delete mach;

    // read complex.mdf and spot check the created machine
    const string complexFile = "." + DIR_SEP + "data" + DIR_SEP + 
        "complex.mdf";
    serializer->setSourceFile(complexFile);
    mach = serializer->readMachine();

    TS_ASSERT(!mach->triggerInvalidatesResults());
    TS_ASSERT(mach->alwaysWriteResults());
    TS_ASSERT(!mach->isFUOrdered());

    // check bus "B1"
    Bus* b1 = mach->busNavigator().item("B1");
    TS_ASSERT(b1->width() == 32);
    TS_ASSERT(b1->immediateWidth() == 8);
    TS_ASSERT(b1->signExtends());
    TS_ASSERT(b1->segmentCount() == 3);
    TS_ASSERT(b1->segment(0)->name() == "seg1");
    TS_ASSERT(b1->segment(1)->name() == "seg2");
    TS_ASSERT(b1->segment(2)->name() == "seg3");
    TS_ASSERT(b1->guardCount() == 2);
    for (int i = 0; i < b1->guardCount(); i++) {
        Guard* guard = b1->guard(i);
        RegisterGuard* rGuard = dynamic_cast<RegisterGuard*>(guard);
        PortGuard* pGuard = dynamic_cast<PortGuard*>(guard);
        if (rGuard != NULL) {
            TS_ASSERT(rGuard->registerFile()->name() == "integer");
            TS_ASSERT(rGuard->registerIndex() == 1);
        } else if (pGuard != NULL) {
            TS_ASSERT(pGuard->port()->name() == "P1");
            TS_ASSERT(pGuard->port()->parentUnit()->name() == "add");
        }
    }

    // check socket "S6"
    Socket* s6 = mach->socketNavigator().item("S6");
    TS_ASSERT(s6->direction() == Socket::INPUT);
    TS_ASSERT(s6->segmentCount() == 4);

    // check bridge "Br5"
    Bridge* br5 = mach->bridgeNavigator().item("Br5");
    TS_ASSERT(br5->sourceBus()->name() == "B8");
    TS_ASSERT(br5->destinationBus()->name() == "B9");

    // check function unit "add_complex"
    FunctionUnit* acFU = mach->functionUnitNavigator().item("add_complex");
    TS_ASSERT(acFU->portCount() == 6);
    TS_ASSERT(acFU->addressSpace()->name() == "AS1");
    BaseFUPort* p8 = acFU->port("P8");
    TS_ASSERT(!p8->isTriggering());
    TS_ASSERT(!p8->isOpcodeSetting());
    TS_ASSERT(p8->width() == 32);
    TS_ASSERT(p8->socketCount() == 2);
    TS_ASSERT(p8->inputSocket()->name() == "S6");
    TS_ASSERT(p8->outputSocket()->name() == "S7");
    FUPort* fp8 = dynamic_cast<FUPort*>(p8);
    TS_ASSERT(fp8->noRegister());
    BaseFUPort* p5 = acFU->port("P5");    
    TS_ASSERT(p5->isTriggering());
    TS_ASSERT(p5->isOpcodeSetting());
    TS_ASSERT(p5->width() == 32);
    TS_ASSERT(p5->socketCount() == 1);
    TS_ASSERT(p5->inputSocket()->name() == "S5");
    FUPort* fp5 = dynamic_cast<FUPort*>(p5);    
    TS_ASSERT(!fp5->noRegister());

	// FU add defintion is in ADF file before the add_complex, check the order
    FunctionUnit* addFU = mach->functionUnitNavigator().item("add");
    TS_ASSERT(addFU->orderNumber() < acFU->orderNumber());
	TS_ASSERT(addFU->orderNumber() == 1);
    TS_ASSERT(acFU->orderNumber() == 2);
    
    // check immediate unit "imm"
    ImmediateUnit* iuImm = mach->immediateUnitNavigator().item("imm");
    TS_ASSERT(iuImm->numberOfRegisters() == 4);
    TS_ASSERT(iuImm->width() == 32);
    TS_ASSERT(iuImm->extensionMode() == Machine::ZERO);
    TS_ASSERT(iuImm->latency() == 1);
    TS_ASSERT(iuImm->portCount() == 1);

    // check instruction template "temp"
    InstructionTemplate* itTemp = mach->instructionTemplateNavigator().item(
        "temp");
    TS_ASSERT(itTemp->supportedWidth(*iuImm) == 32);
    
    // check global control unit "gcu"
    ControlUnit* gcu = mach->controlUnit();
    TS_ASSERT(gcu->name() == "gcu");
    TS_ASSERT(gcu->hasPort("control"));
    TS_ASSERT(gcu->hasReturnAddressPort());
    TS_ASSERT(gcu->returnAddressPort()->name() == "ra");
    TS_ASSERT(gcu->delaySlots() == 0);
    TS_ASSERT(gcu->addressSpace()->name() == "AS1");
    TS_ASSERT(gcu->hasOperation("jump"));

    // check address space "AS2"
    AddressSpace* as2 = mach->addressSpaceNavigator().item("AS2");
    TS_ASSERT(as2->width() == 32);
    TS_ASSERT(as2->start() == 1);
    TS_ASSERT(as2->end() == 32);
    
    delete mach;
    delete serializer;
}

#endif
