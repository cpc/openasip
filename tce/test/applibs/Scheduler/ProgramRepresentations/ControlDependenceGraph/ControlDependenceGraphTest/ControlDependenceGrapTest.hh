/**
 * @file ControlDependenceGraphTest.hh
 *
 * A test suite for Control Dependence analysis.
 *
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef CONTROL_DEPENDENCE_GRAPH_TEST_HH
#define CONTROL_DEPENDENCE_GRAPH_TEST_HH

#include <iostream>

#include <TestSuite.h>
#include "ControlDependenceGraph.hh"
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

class ControlDependenceGraphTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testProcedureCDG();
    void testRallocatedCDG();
    void testImmediatesCDG();

};


void
ControlDependenceGraphTest::setUp() {
}

void
ControlDependenceGraphTest::tearDown() {
}


/**
 * Tests
 *
 * @todo
 */
void
ControlDependenceGraphTest::testProcedureCDG() {

    OperationPool opool;
    UniversalMachine umach(opool);
    TPEF::BinaryStream binaryStream("data/arrmul.tpef");

    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);

    assert(tpef_ != NULL);

    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(*tpef_, umach);
    TTAProgram::Program* currentProgram = factory.build();

    ControlFlowGraph CFG(currentProgram->procedure(1));
    ControlDependenceGraph CDG(CFG);
    TS_ASSERT_EQUALS(CDG.nodeCount(), 6);
    TS_ASSERT_EQUALS(CDG.edgeCount(), 6);
    CFG.writeToDotFile("/dev/null");
    CDG.writeToDotFile("/dev/null");
    
    ControlFlowGraph CFG2(currentProgram->procedure(2));
    ControlDependenceGraph CDG2(CFG2);
    TS_ASSERT_EQUALS(CDG2.nodeCount(), 4);
    TS_ASSERT_EQUALS(CDG2.edgeCount(), 3);
    CFG2.writeToDotFile("/dev/null");    
    CDG2.writeToDotFile("/dev/null");

    delete currentProgram;
    currentProgram = NULL;

}

void
ControlDependenceGraphTest::testRallocatedCDG() {
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

    ControlFlowGraph CFG(currentProgram->procedure(1));
    ControlDependenceGraph CDG(CFG);
    TS_ASSERT_EQUALS(CDG.nodeCount(), 6);
    TS_ASSERT_EQUALS(CDG.edgeCount(), 6);
    CDG.writeToDotFile("/dev/null");
    CFG.writeToDotFile("/dev/null");
    
    ControlFlowGraph CFG2(currentProgram->procedure(2));
    ControlDependenceGraph CDG2(CFG2);
    TS_ASSERT_EQUALS(CDG2.nodeCount(), 4);
    TS_ASSERT_EQUALS(CDG2.edgeCount(), 3);
    CFG2.writeToDotFile("/dev/null");
    CDG2.writeToDotFile("/dev/null");


    delete currentProgram;
    currentProgram = NULL;
}

void
ControlDependenceGraphTest::testImmediatesCDG() {
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

    ControlFlowGraph CFG0(currentProgram->procedure(0));
    ControlDependenceGraph CDG0(CFG0);
    TS_ASSERT_EQUALS(CDG0.nodeCount(), 4);
    TS_ASSERT_EQUALS(CDG0.edgeCount(), 3);
    CFG0.writeToDotFile("/dev/null");
    CDG0.writeToDotFile("/dev/null");
    
    ControlFlowGraph CFG1(currentProgram->procedure(1));
    ControlDependenceGraph CDG1(CFG1);
    TS_ASSERT_EQUALS(CDG1.nodeCount(), 9);
    TS_ASSERT_EQUALS(CDG1.edgeCount(), 9);
    CFG1.writeToDotFile("/dev/null");
    CDG1.writeToDotFile("/dev/null");
    
    ControlFlowGraph CFG2(currentProgram->procedure(2));    
    ControlDependenceGraph CDG2(CFG2);    
    TS_ASSERT_EQUALS(CDG2.nodeCount(), 11);
    TS_ASSERT_EQUALS(CDG2.edgeCount(), 11);
    CFG2.writeToDotFile("/dev/null");
    CDG2.writeToDotFile("/dev/null");


    delete currentProgram;
    currentProgram = NULL;
}

#endif
