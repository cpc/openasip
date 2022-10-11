/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file FUResourceConflictDetectorTest.hh
 * 
 * A test suite for FUResourceConflictDetector.
 *
 * @author Pekka Jääskeläinen 2006,2010 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef RESOURCE_CONFLICT_DETECTOR_TEST_HH
#define RESOURCE_CONFLICT_DETECTOR_TEST_HH

#include <TestSuite.h>
#include "FUResourceConflictDetector.hh"
#include "FSAFUResourceConflictDetector.hh"
#include "ResourceVectorFUResourceConflictDetector.hh"
#include "FunctionUnit.hh"
#include "ADFSerializer.hh"
#include "ConflictDetectingOperationExecutor.hh"
#include "MachineStateBuilder.hh"
#include "MachineState.hh"
#include "MemorySystem.hh"
#include "FUConflictDetectorIndex.hh"
#include "AssocTools.hh"
#include "ReservationTableFUResourceConflictDetector.hh"
#include "DCMFUResourceConflictDetector.hh"
#include "TCEString.hh"
#include "Machine.hh"
#include "Conversion.hh"

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
        trace_ << prefix_ << operationName(id) << separator_;
        return FSAFUResourceConflictDetector::issueOperation(id);
    }    

    std::string trace() {
        return trace_;
    }
private:
    TCEString trace_;
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
        machineState = builder.build(*testMachine, dummyMS, detectors));

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
        machineState = builder.build(*testMachine, dummyMS, detectors));

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
