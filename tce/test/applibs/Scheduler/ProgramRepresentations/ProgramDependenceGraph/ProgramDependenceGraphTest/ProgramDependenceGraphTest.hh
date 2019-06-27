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
 * @file ProgramGraphTest.hh
 *
 * A test suite for Program Dependence analysis.
 *
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef PROGRAM_GRAPH_TEST_HH
#define PROGRAM_GRAPH_TEST_HH

#include <iostream>

#include <TestSuite.h>
#include "ProgramGraph.hh"
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

class ProgramGraphTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testProcedureGraph();
    void testRallocatedGraph();
    void testImmediatesGraph();

};


void
ProgramGraphTest::setUp() {
}

void
ProgramGraphTest::tearDown() {
}


/**
 * Tests
 *
 * @todo
 */
void
ProgramGraphTest::testProcedureGraph() {

    UniversalMachine* umach = &UniversalMachine::instance();
    TPEF::BinaryStream binaryStream("data/arrmul.tpef");

    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);

    assert(tpef_ != NULL);

    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(*tpef_, umach);
    TTAProgram::Program* currentProgram = factory.build();

    try {
        ProgramGraph graph(*currentProgram, *umach);
        TS_ASSERT_EQUALS(graph.graphCount(), 3);
        for (int i = 0; i < graph.graphCount(); i++){
            TS_ASSERT_THROWS_NOTHING(graph.graph(graph.graphAt(i)->name()));
        }
        TS_ASSERT_THROWS_NOTHING(graph.graph("_arrmul"));
        TS_ASSERT_THROWS(graph.graph("__arrmul"),InvalidData); 
    } catch (const Exception& e) {
        throw InvalidData(
            __FILE__, __LINE__, __func__, e.errorMessageStack());        
    }
    
    delete currentProgram;
    currentProgram = NULL;

}

void
ProgramGraphTest::testRallocatedGraph() {

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

    try {
        ProgramGraph graph(*currentProgram, *machine);
        TS_ASSERT_EQUALS(graph.graphCount(), 3);
        TS_ASSERT_THROWS_NOTHING(graph.graph("_crt0"));
        TS_ASSERT_THROWS(graph.graph("__crt0"),InvalidData); 
    } catch (const Exception& e) {
        throw InvalidData(
            __FILE__, __LINE__, __func__, e.errorMessageStack());
    }

    delete currentProgram;
    currentProgram = NULL;
}

void
ProgramGraphTest::testImmediatesGraph() {

    try {
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
        
        TS_ASSERT_THROWS(ProgramGraph graph(*currentProgram, *machine), 
                         InvalidData);

        delete currentProgram;
        currentProgram = NULL;
    } catch (const Exception& e) {
        throw InvalidData(
            __FILE__, __LINE__, __func__, e.errorMessageStack());
    }    
}

#endif
