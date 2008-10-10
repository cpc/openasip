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
 * @file FUResourceConflictDetectorTest.hh
 * 
 * A test suite for FUResourceConflictDetector.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef RESOURCE_CONFLICT_DETECTOR_TEST_HH
#define RESOURCE_CONFLICT_DETECTOR_TEST_HH

#include <TestSuite.h>
#include "FUResourceConflictDetector.hh"
#include "FSAFUResourceConflictDetector.hh"
#include "ResourceVectorFUResourceConflictDetector.hh"
#include "FunctionUnit.hh"
#include "ADFSerializer.hh"
#include "GlobalLock.hh"
#include "ConflictDetectingOperationExecutor.hh"
#include "MachineStateBuilder.hh"
#include "MachineState.hh"
#include "MemorySystem.hh"
#include "FUConflictDetectorIndex.hh"
#include "AssocTools.hh"
#include "ReservationTableFUResourceConflictDetector.hh"
#include "DCMFUResourceConflictDetector.hh"
#include "TCEString.hh"

/**
 * Class for testing FU resource conflict detectors.
 */
class FUResourceConflictDetectorTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    
    void testBaseClass();
    void testFSAImplementation();
    void testResourceVectorImplementation();
    void testReservationTableImplementation();
    void testDCMImplementation();
    void testInsideFUState();
    void testInsideFUState2();

private:
    TTAMachine::FunctionUnit* fullPipeAluFU;
    TTAMachine::FunctionUnit* mixedLatencyAluFU;
    TTAMachine::FunctionUnit* simpleFU;
    TTAMachine::FunctionUnit* complexFU;

    void simulateComplexFU(FUResourceConflictDetector& fsa);
    void advanceClockOfAllDetectors(FUConflictDetectorIndex& detectors);
};

const std::string FU_ADF = "data/test_fus.adf";

/**
 * A derived detector that keeps book of the issued operations.
 * 
 * For verification.
 */
class TrackedDetector : public FSAFUResourceConflictDetector {
public:
    TrackedDetector(
        const std::string prefix,
        const std::string separator,
        const TTAMachine::FunctionUnit& fu, 
        bool lazyFSA = true) : 
        FSAFUResourceConflictDetector(fu),
        prefix_(prefix), separator_(separator) {
        if (!lazyFSA)
            initializeAllStates();
    }

    virtual bool issueOperation(OperationID id) {
        trace_ += prefix_ + operationName(id) + separator_;
        return FSAFUResourceConflictDetector::issueOperation(id);
    }    

    std::string trace() {
        return trace_;
    }
private:
    std::string trace_;
    std::string prefix_;
    std::string separator_;
};

/**
 * Called before each test.
 */
void
FUResourceConflictDetectorTest::setUp() {

    ADFSerializer serializer;
    serializer.setSourceFile(FU_ADF);

    TTAMachine::Machine* testMachine = NULL;
    CATCH_ANY(testMachine = serializer.readMachine());
    CATCH_ANY(fullPipeAluFU = 
              testMachine->functionUnitNavigator().item("full_pipe_alu"));
    CATCH_ANY(mixedLatencyAluFU = 
              testMachine->functionUnitNavigator().item("mixed_latency"));
    CATCH_ANY(simpleFU = 
              testMachine->functionUnitNavigator().item("simple_mul"));
    CATCH_ANY(complexFU = 
              testMachine->functionUnitNavigator().item("complex"));
}


/**
 * Called after each test.
 */
void
FUResourceConflictDetectorTest::tearDown() {
}

/**
 * Test the base class.
 */
void
FUResourceConflictDetectorTest::testBaseClass() {

    FUResourceConflictDetector detector;
    TS_ASSERT(detector.issueOperation(12351));
    TS_ASSERT(detector.advanceCycle());
}

/**
 * Simulates the complexFU using the given resource conflict detector.
 *
 * @param detector The detector to simulate the FU with.
 */
void 
FUResourceConflictDetectorTest::simulateComplexFU(
    FUResourceConflictDetector& detector) {

    FUResourceConflictDetector::OperationID ADDF = 0;
    FUResourceConflictDetector::OperationID MULF = 0;
    FUResourceConflictDetector::OperationID DIVF = 0;

    TS_ASSERT_THROWS_NOTHING(ADDF = detector.operationID("ADDF"));
    TS_ASSERT_THROWS_NOTHING(MULF = detector.operationID("MULF"));
    TS_ASSERT_THROWS_NOTHING(DIVF = detector.operationID("DIVF"));

#define ISSUE_OK(OPERATION__) \
    TS_ASSERT(detector.issueOperation(OPERATION__)); \
    TS_ASSERT(detector.advanceCycle())

#define ISSUE_NOK(OPERATION__) \
    TS_ASSERT(!detector.issueOperation(OPERATION__)); \
    detector.reset()

#define NEXT_CYCLE() \
    TS_ASSERT(detector.advanceCycle())

    ISSUE_OK(ADDF);
    ISSUE_NOK(ADDF); // back to S0

    ISSUE_OK(ADDF);
    ISSUE_NOK(DIVF); // back to S0

    ISSUE_OK(ADDF);

    NEXT_CYCLE();
    NEXT_CYCLE();
    NEXT_CYCLE();

    ISSUE_NOK(ADDF); 
    ISSUE_OK(MULF);
    
    NEXT_CYCLE();

#undef ISSUE_OK
#undef ISSUE_NOK
#undef NEXT_CYCLE

}

/**
 * Test the FSA implementation.
 */
void
FUResourceConflictDetectorTest::testFSAImplementation() {

    FSAFUResourceConflictDetector detector(*complexFU);
    simulateComplexFU(detector);
}

/**
 * Test the resource vector implementation.
 */
void
FUResourceConflictDetectorTest::testResourceVectorImplementation() {

    ResourceVectorFUResourceConflictDetector detector(*complexFU);
    simulateComplexFU(detector);
}

/**
 * Test the reservation table implementation.
 */
void
FUResourceConflictDetectorTest::testReservationTableImplementation() {

    ReservationTableFUResourceConflictDetector detector(*complexFU);
    simulateComplexFU(detector);
}

/**
 * Test the DCM implementation.
 */
void
FUResourceConflictDetectorTest::testDCMImplementation() {

    DCMFUResourceConflictDetector detector(*complexFU);
    simulateComplexFU(detector);
}

void
FUResourceConflictDetectorTest::advanceClockOfAllDetectors(
    FUConflictDetectorIndex& detectors) {
    for (FUConflictDetectorIndex::const_iterator i = detectors.begin();
         i != detectors.end(); ++i) {
        if (!(*i).second->isIdle())
            (*i).second->advanceClock();
    }
}

/**
 * Test the detection when used inside an FUState object.
 */
void
FUResourceConflictDetectorTest::testInsideFUState() {

    ADFSerializer serializer;
    serializer.setSourceFile(FU_ADF);

    TTAMachine::Machine* testMachine = NULL;
    CATCH_ANY(testMachine = serializer.readMachine());

    MemorySystem dummyMS(*testMachine);

    GlobalLock lock;
    MachineStateBuilder builder;
    MachineState* machineState = NULL;

    FUConflictDetectorIndex detectors;

    detectors["full_pipe_alu"] = 
        new FSAFUResourceConflictDetector(
            *testMachine->functionUnitNavigator().item("full_pipe_alu"));

    detectors["mixed_latency"] = 
        new FSAFUResourceConflictDetector(
            *testMachine->functionUnitNavigator().item("mixed_latency"));

    detectors["complex"] = 
        new FSAFUResourceConflictDetector(
            *testMachine->functionUnitNavigator().item("complex"));

    detectors["simple_mul"] = 
        new ResourceVectorFUResourceConflictDetector(
            *testMachine->functionUnitNavigator().item("simple_mul"));


    CATCH_ANY(
        machineState = builder.build(*testMachine, dummyMS, lock, detectors));

    OperationPool opPool;

#define EXEC_OP(FU_NAME__, OP_NAME__) \
    machineState->fuState(FU_NAME__).setOperation(opPool.operation(OP_NAME__));\
    machineState->fuState(FU_NAME__).setTriggered()

#define NEXT_CYCLE() \
    machineState->endClockOfAllFUStates();\
    machineState->advanceClockOfAllFUStates();\
    advanceClockOfAllDetectors(detectors)

    EXEC_OP("complex", "ADDF");
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE());

    EXEC_OP("complex", "ADDF");
    TS_ASSERT_THROWS(NEXT_CYCLE(),  SimulationExecutionError);

    NEXT_CYCLE();// ensure we are get at S0

    EXEC_OP("complex", "ADDF");
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE());

    EXEC_OP("complex", "DIVF");
    TS_ASSERT_THROWS(NEXT_CYCLE(),  SimulationExecutionError);

    NEXT_CYCLE();
    NEXT_CYCLE();
    NEXT_CYCLE();

    EXEC_OP("complex", "DIVF");
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE());

    EXEC_OP("complex", "ADDF");
    TS_ASSERT_THROWS(NEXT_CYCLE(),  SimulationExecutionError);

    NEXT_CYCLE();

    EXEC_OP("complex", "MULF");
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE());

    AssocTools::deleteAllValues(detectors);
}

/**
 * Test the detection when used inside an FUState object.
 *
 * Trying to reproduce the bug in #349
 */
void
FUResourceConflictDetectorTest::testInsideFUState2() {

    ADFSerializer serializer;
    serializer.setSourceFile(FU_ADF);

    TTAMachine::Machine* testMachine = NULL;
    CATCH_ANY(testMachine = serializer.readMachine());

    MemorySystem dummyMS(*testMachine);

    GlobalLock lock;
    MachineStateBuilder builder;
    MachineState* machineState = NULL;

    FUConflictDetectorIndex detectors;

    TrackedDetector* detector = 
        new TrackedDetector(
            "", "|",
            *testMachine->functionUnitNavigator().item("mixed_latency"),
            false);

    detectors["mixed_latency"] = detector;

    CATCH_ANY(
        machineState = builder.build(*testMachine, dummyMS, lock, detectors));

    OperationPool opPool;

    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE());
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE());
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE());
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE());

    // PC:78880
    EXEC_OP("mixed_latency", "ADD");
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE()); //->S1

    // PC:78881
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE()); //->S0

    // PC:78882
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE()); //(idle)

    // PC:78883
    EXEC_OP("mixed_latency", "MUL");
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE()); //->S3 

    // PC:78884
    EXEC_OP("mixed_latency", "MUL");
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE()); //->S4

    // PC:78885
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE()); //->S0

    // PC:78886
    EXEC_OP("mixed_latency", "MUL");
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE()); //->S3

    // PC:78887 (the conflict occurs)
    EXEC_OP("mixed_latency", "ADD");
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE()); //->S2

    // PC:78888
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE()); //->S6

    // PC:78889
    EXEC_OP("mixed_latency", "MUL");
    TS_ASSERT_THROWS_NOTHING(NEXT_CYCLE()); //->S3
    
    TS_ASSERT_EQUALS(detector->trace(), "ADD|MUL|MUL|MUL|ADD|MUL|");

#if 0
    detector->writeToDotFile("fsa.dot");
#endif
}
#endif
