/**
 * @file ProgramGraphTest.hh
 *
 * A test suite for Program Dependence analysis.
 *
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
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

    OperationPool opool;
    UniversalMachine umach(opool);
    TPEF::BinaryStream binaryStream("data/arrmul.tpef");

    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);

    assert(tpef_ != NULL);

    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(*tpef_, umach);
    TTAProgram::Program* currentProgram = factory.build();

    try {
        ProgramGraph graph(*currentProgram);
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

    try {
        ProgramGraph graph(*currentProgram);
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
        
        TS_ASSERT_THROWS(ProgramGraph graph(*currentProgram), OutOfRange);

        delete currentProgram;
        currentProgram = NULL;
    } catch (const Exception& e) {
        throw InvalidData(
            __FILE__, __LINE__, __func__, e.errorMessageStack());
    }
    
}

#endif
