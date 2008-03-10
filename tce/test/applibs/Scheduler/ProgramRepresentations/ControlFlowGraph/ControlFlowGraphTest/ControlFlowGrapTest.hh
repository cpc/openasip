/**
 * @file ControlFlowGraphTest.hh
 *
 * A test suite for Control Flow analysis.
 *
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
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

    OperationPool opool;
    UniversalMachine umach(opool);
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
    UniversalMachine umach;
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
    UniversalMachine umach;
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
