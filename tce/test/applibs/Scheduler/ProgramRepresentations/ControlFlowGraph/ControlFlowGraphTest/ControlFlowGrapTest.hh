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
 * @file ControlFlowGraphTest.hh
 *
 * A test suite for Control Flow analysis.
 *
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef CONTROL_FLOW_GRAPH_TEST_HH
#define CONTROL_FLOW_GRAPH_TEST_HH

#include <iostream>

#include <TestSuite.h>
#include "ControlFlowGraph.hh"
#include "BinaryStream.hh"
#include "TPEFProgramFactory.hh"
#include "Program.hh"
#include "BinaryReader.hh"
#include "UniversalMachine.hh"
#include "OperationPool.hh"
#include "MoveNode.hh"
#include "ADFSerializer.hh"
#include "Instruction.hh"

using TTAProgram::Move;

class ControlFlowGraphTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testProcedureCFG();
    void testRallocatedCFG();
    void testImmediatesCFG();

};


void
ControlFlowGraphTest::setUp() {
}

void
ControlFlowGraphTest::tearDown() {
}


/**
 * Tests
 *
 * @todo
 */
void
ControlFlowGraphTest::testProcedureCFG() {

    UniversalMachine* umach = &UniversalMachine::instance();
    TPEF::BinaryStream binaryStream("data/arrmul.tpef");

    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);

    assert(tpef_ != NULL);

    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(*tpef_, umach);
    TTAProgram::Program* currentProgram = factory.build();

    ControlFlowGraph cfg(currentProgram->procedure(1));
    TS_ASSERT_EQUALS(cfg.nodeCount(), 5);
    TS_ASSERT_EQUALS(cfg.edgeCount(), 6);
    cfg.writeToDotFile("/dev/null");
    ControlFlowGraph cfg2(currentProgram->procedure(2));
    TS_ASSERT_EQUALS(cfg2.nodeCount(), 4);
    TS_ASSERT_EQUALS(cfg2.edgeCount(), 3);
    cfg2.writeToDotFile("/dev/null");

    delete currentProgram;
    currentProgram = NULL;

}

void
ControlFlowGraphTest::testRallocatedCFG() {
    UniversalMachine* umach = &UniversalMachine::instance();
    TPEF::BinaryStream binaryStream("data/rallocated_arrmul.tpef");
    ADFSerializer adfSerializer;
    adfSerializer.setSourceFile("data/10_bus_full_connectivity.adf");

    TTAMachine::Machine* machine = adfSerializer.readMachine();

    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);

    assert(tpef_ != NULL);

    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(*tpef_, *machine, umach);
    TTAProgram::Program* currentProgram = factory.build();

    ControlFlowGraph cfg(currentProgram->procedure(1));
    TS_ASSERT_EQUALS(cfg.nodeCount(), 5);
    TS_ASSERT_EQUALS(cfg.edgeCount(), 6);
    cfg.writeToDotFile("/dev/null");
    ControlFlowGraph cfg2(currentProgram->procedure(2));
    TS_ASSERT_EQUALS(cfg2.nodeCount(), 4);
    TS_ASSERT_EQUALS(cfg2.edgeCount(), 3);
    cfg2.writeToDotFile("/dev/null");


    delete currentProgram;
    currentProgram = NULL;
}

void
ControlFlowGraphTest::testImmediatesCFG() {

    UniversalMachine* umach = &UniversalMachine::instance();
    TPEF::BinaryStream
        binaryStream("data/3_bus_reduced_connectivity_shortimms.tpef");
    ADFSerializer adfSerializer;
    adfSerializer.setSourceFile(
        "data/3_bus_reduced_connectivity_shortimms.adf");

    TTAMachine::Machine* machine = adfSerializer.readMachine();

    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);

    assert(tpef_ != NULL);

    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(*tpef_, *machine, umach);
    TTAProgram::Program* currentProgram = factory.build();

    ControlFlowGraph cfg0(currentProgram->procedure(0));
    TS_ASSERT_EQUALS(cfg0.nodeCount(), 4);
    TS_ASSERT_EQUALS(cfg0.edgeCount(), 3);
    cfg0.writeToDotFile("/dev/null");
    ControlFlowGraph cfg1(currentProgram->procedure(1));
    TS_ASSERT_EQUALS(cfg1.nodeCount(), 7);
    TS_ASSERT_EQUALS(cfg1.edgeCount(), 7);
    cfg1.writeToDotFile("/dev/null");
    ControlFlowGraph cfg2(currentProgram->procedure(2));
    TS_ASSERT_EQUALS(cfg2.nodeCount(), 9);
    TS_ASSERT_EQUALS(cfg2.edgeCount(), 9);
    cfg2.writeToDotFile("/dev/null");


    delete currentProgram;
    currentProgram = NULL;
}

#endif
