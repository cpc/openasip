/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file MachineStateBuilderTest.hh
 * 
 * A test suite for MachineStateBuilder.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005,2010 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef MACHINE_STATE_BUILDER_TEST_HH
#define MACHINE_STATE_BUILDER_TEST_HH

#include <TestSuite.h>

#include "MachineStateBuilder.hh"
#include "Machine.hh"
#include "ADFSerializer.hh"
#include "FUState.hh"
#include "MachineState.hh"
#include "PortState.hh"
#include "InputPortState.hh"
#include "TriggeringInputPortState.hh"
#include "OutputPortState.hh"
#include "OpcodeSettingVirtualInputPortState.hh"
#include "RegisterFileState.hh"
#include "BusState.hh"
#include "MemoryAccessingFUState.hh"
#include "MemorySystem.hh"
#include "IdealSRAM.hh"

using namespace TTAMachine;

/**
 * Class for testing MachineStateBuilder.
 */
class MachineStateBuilderTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testBuildMachineState();
};


/**
 * Called before each test.
 */
void
MachineStateBuilderTest::setUp() {
}


/**
 * Called after each test.
 */
void
MachineStateBuilderTest::tearDown() {
}

/**
 * Test that machine state building is successful.
 */
void
MachineStateBuilderTest::testBuildMachineState() {

    ADFSerializer serializer;
    serializer.setSourceFile("data/test_machine.adf");

    Machine* machine = NULL;
    TS_ASSERT_THROWS_NOTHING(machine = serializer.readMachine());

    Machine::AddressSpaceNavigator navigator = 
        machine->addressSpaceNavigator();
    AddressSpace* as1 = navigator.item("AS1");

    MemorySystem memSys(*machine);
    MemorySystem::MemoryPtr sram = 
        MemorySystem::MemoryPtr(
            new IdealSRAM(as1->start(), as1->end(), 8));
    TS_ASSERT_THROWS_NOTHING(memSys.addAddressSpace(*as1, sram));

    MachineStateBuilder builder;

    MachineState* state = builder.build(*machine, memSys);

    TS_ASSERT_EQUALS(state->FUStateCount(), 2);
    
    FUState& fu = state->fuState("FU_1");
    TS_ASSERT(&fu != &NullFUState::instance());

    FUState& fu2 = state->fuState("Memory_fu");
    TS_ASSERT(dynamic_cast<MemoryAccessingFUState*>(&fu2) != NULL);

    PortState& port1 = state->portState("P1", "FU_1");
    PortState& port2 = state->portState("P2", "FU_1");
    PortState& port3 = state->portState("P3", "FU_1");
    PortState& port4 = state->portState("P4", "FU_1");
    PortState& port5 = state->portState("P2.ADD", "FU_1");
    PortState& port6 = state->portState("P2.SUB", "FU_1");

    TS_ASSERT(dynamic_cast<InputPortState*>(&port1) != NULL);
    TS_ASSERT(dynamic_cast<TriggeringInputPortState*>(&port2) != NULL);
    TS_ASSERT(dynamic_cast<OutputPortState*>(&port3) != NULL);
    TS_ASSERT(dynamic_cast<OutputPortState*>(&port4) != NULL);
    TS_ASSERT(dynamic_cast<OpcodeSettingVirtualInputPortState*>(&port5) != 
              NULL);
    TS_ASSERT(dynamic_cast<OpcodeSettingVirtualInputPortState*>(&port6) != 
              NULL);

    PortState& port7 = state->portState("P1", "Memory_fu");
    PortState& port8 = state->portState("P2", "Memory_fu");
    PortState& port9 = state->portState("P3", "Memory_fu");
    PortState& port10 = state->portState("P2.LDW", "Memory_fu");
    PortState& port11 = state->portState("P2.STW", "Memory_fu");

    TS_ASSERT(dynamic_cast<InputPortState*>(&port7) != NULL);
    TS_ASSERT(dynamic_cast<TriggeringInputPortState*>(&port8) != NULL);
    TS_ASSERT(dynamic_cast<OutputPortState*>(&port9) != NULL);
    TS_ASSERT(dynamic_cast<OpcodeSettingVirtualInputPortState*>(&port10) != 
              NULL);
    TS_ASSERT(dynamic_cast<OpcodeSettingVirtualInputPortState*>(&port11) != 
              NULL);

    RegisterFileState& regState = state->registerFileState("RF");
    TS_ASSERT(&regState != &NullRegisterFileState::instance());

    BusState& bus1 = state->busState("B1");
    BusState& bus2 = state->busState("B2");

    TS_ASSERT(&bus1 != &NullBusState::instance());
    TS_ASSERT(&bus2 != &NullBusState::instance());

    // try to simulate the functioning of the built machine state
    SimValue value1(32);
    SimValue value2(32);
    value1 = 10;
    value2 = 20;
    port1.setValue(value1);
    port5.setValue(value2);

    fu.endClock();
    fu.advanceClock();

    TS_ASSERT(port3.value() == 30);

    value1 = 50;
    regState.registerState(5).setValue(value1);
    TS_ASSERT_EQUALS(regState.registerState(5).value(), 50);

    value1 = 100;
    bus1.setValue(value1);
    TS_ASSERT_EQUALS(bus1.value(), 100);

    delete machine;
    delete state;

}

#endif
