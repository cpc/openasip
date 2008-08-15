/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
