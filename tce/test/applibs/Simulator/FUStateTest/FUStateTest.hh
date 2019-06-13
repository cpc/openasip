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
 * @file FUStateTest.hh
 * 
 * A test suite for FUState.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005,2010 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef FU_STATE_TEST_HH
#define FU_STATE_TEST_HH

#include <TestSuite.h>
#include <iostream>
#include <cfloat>
#include <boost/array.hpp>

#include "Machine.hh"
#include "FUState.hh"
#include "InputPortState.hh"
#include "OutputPortState.hh"
#include "TriggeringInputPortState.hh"
#include "OpcodeSettingVirtualInputPortState.hh"
#include "SimValue.hh"
#include "OperationPool.hh"
#include "Operation.hh"
#include "OperationContext.hh"
#include "OneCycleOperationExecutor.hh"
#include "SimpleOperationExecutor.hh"
#include "IdealSRAM.hh"
#include "MemoryAccessingFUState.hh"
#include "MachineStateBuilder.hh"
#include "MemorySystem.hh"
#include "MachineState.hh"
#include "FUResourceConflictDetector.hh"
#include "ResourceVectorFUResourceConflictDetector.hh"
#include "FSAFUResourceConflictDetector.hh"
#include "Operation.hh"
#include "OperationPool.hh"
#include "ExecutableMove.hh"
#include "BuslessExecutableMove.hh"
#include "AssocTools.hh"
#include "StringTools.hh"

using std::cout;
using std::endl;

// define this if want to run the long running benchmark of the resource
// conflict detection models (takes hours to complete)
// #define CONFLICT_DETECTOR_BENCHMARK 

/**
 * Test class for FUState.
 */
class FUStateTest : public CxxTest::TestSuite {
public:

    FUStateTest();
    virtual ~FUStateTest();

    void setUp();
    void tearDown();

    void testOneCycleOperationExecutor();
    void testSimpleOperationExecutor();
    void testMemoryAccessingFUState();

    void testConflictDetectionModelBenchmark();

#ifdef CONFLICT_DETECTOR_BENCHMARK 
public:
    void benchmarkSimulateNone();
    void benchmarkSimulateResourceVectors();
    void benchmarkSimulateActiveFSA();
    void benchmarkSimulateLazyFSA();

    void benchmarkInitNone();
    void benchmarkInitResourceVectors();
    void benchmarkInitLazyFSA();
    void benchmarkInitActiveFSA();


private:
    const TTAMachine::Machine* aluMach;
    const TTAMachine::Machine* mulMach;
    const TTAMachine::Machine* fpuMach;
    boost::array<Operation*, 17> aluOperations;
    boost::array<Operation*, 1> mulOperations;
    boost::array<Operation*, 8> fpuOperations;
#endif


};

/**
 * Constructor (called once before running any tests).
 */
FUStateTest::FUStateTest() {
#ifdef CONFLICT_DETECTOR_BENCHMARK 
    std::cout 
        << std::endl 
        << "Loading FUs from ADF files....";

    aluMach = TTAMachine::Machine::loadFromADF("data/alu.adf");
    mulMach = TTAMachine::Machine::loadFromADF("data/mul.adf");
    fpuMach = TTAMachine::Machine::loadFromADF("data/fpu.adf");

    std::cout << "OK." << std::endl;

    OperationPool operations;
    boost::array<Operation*, 17> aluOps = {
        &operations.operation("abs"),  &operations.operation("add"), 
        &operations.operation("and"),  &operations.operation("eq"), 
        &operations.operation("gt"),   &operations.operation("gtu"), 
        &operations.operation("ior"),  &operations.operation("max"), 
        &operations.operation("maxu"), &operations.operation("min"), 
        &operations.operation("shl"),  &operations.operation("shr"), 
        &operations.operation("shru"), &operations.operation("sub"), 
        &operations.operation("sxhw"), &operations.operation("sxqw"), 
        &operations.operation("xor")};
    aluOperations = aluOps;

    boost::array<Operation*, 1> mulOps = {
        &operations.operation("mul")};
    mulOperations = mulOps;

    // the MIPS R4000 FPU
    boost::array<Operation*, 8> fpuOps = {
        &operations.operation("addf"), &operations.operation("mulf"), 
        &operations.operation("divf"), &operations.operation("absf"), 
        &operations.operation("eqf"), &operations.operation("negf"), 
        &operations.operation("sqrtf"), &operations.operation("subf")};
    fpuOperations = fpuOps;
#endif
}

/**
 * Destructor.
 */
FUStateTest::~FUStateTest() {
    // to remove noise from Valgrind output
    OperationPool::cleanupCache();
}

/**
 * Called before each test.
 */
void
FUStateTest::setUp() {
}


/**
 * Called after each test.
 */
void
FUStateTest::tearDown() {
}

/**
 * Test that OneCycleOperationExecutor works.
 */
void
FUStateTest::testOneCycleOperationExecutor() {

    OperationPool pool;

    Operation& add = pool.operation("TESTADD");
    Operation& sub = pool.operation("TESTSUB");

    TS_ASSERT_DIFFERS(&add, &NullOperation::instance());
    TS_ASSERT_DIFFERS(&sub, &NullOperation::instance());

    FUState fu;
    OneCycleOperationExecutor executor(fu);
   
    InputPortState port1(fu, 32);
    TriggeringInputPortState port2(fu, 32);
    OpcodeSettingVirtualInputPortState virtual1(add, fu, port2);
    OpcodeSettingVirtualInputPortState virtual2(sub, fu, port2);

    OutputPortState port3(fu, 32);
   
    TS_ASSERT_THROWS_NOTHING(executor.addBinding(1, port1));
    TS_ASSERT_THROWS_NOTHING(executor.addBinding(2, port2));
    TS_ASSERT_THROWS_NOTHING(executor.addBinding(3, port3));

    fu.addOperationExecutor(executor, add);
    fu.addOperationExecutor(executor, sub);

    fu.addInputPortState(port1);
    fu.addInputPortState(port2);
    fu.addInputPortState(virtual1);
    fu.addInputPortState(virtual2);

    fu.addOutputPortState(port3);

    // cycle 1
    SimValue value1(32);
    value1 = 2;
    SimValue value2(32);
    value2 = 2;
    port1.setValue(value1);
    virtual1.setValue(value2);

    TS_ASSERT_EQUALS(port2.value(), 2);

    fu.endClock();
    fu.advanceClock();

    // cycle 2
    TS_ASSERT_EQUALS(port3.value(), 4);

    value1 = 100;
    value2 = 50;
    port1.setValue(value1);
    virtual2.setValue(value2);
    
    fu.endClock();
    fu.advanceClock();

    // cycle 3
    TS_ASSERT_EQUALS(port3.value(), 50);
}

/**
 * Test that SimpleOperationExecutor works.
 */
void
FUStateTest::testSimpleOperationExecutor() {
    OperationPool pool;

    Operation& add = pool.operation("TESTADD");
    TS_ASSERT_DIFFERS(&add, &NullOperation::instance());

    Operation& sub = pool.operation("TESTSUB");
    TS_ASSERT_DIFFERS(&sub, &NullOperation::instance());

    Operation& mul = pool.operation("TESTMUL");
    TS_ASSERT_DIFFERS(&mul, &NullOperation::instance());

    FUState fu;
    SimpleOperationExecutor executor1(7, fu);
    SimpleOperationExecutor executor2(5, fu);
    SimpleOperationExecutor executor3(2, fu);

    InputPortState port1(fu, 32);
    TriggeringInputPortState port2(fu, 32);
    OpcodeSettingVirtualInputPortState virtual1(
        add, fu, port2);
    OpcodeSettingVirtualInputPortState virtual2(
        sub, fu, port2);
    OpcodeSettingVirtualInputPortState virtual3(
        mul, fu, port2);

    OutputPortState output1(fu, 32);

    fu.addInputPortState(port1);
    fu.addInputPortState(port2);
    fu.addInputPortState(virtual1);
    fu.addInputPortState(virtual2);
    fu.addInputPortState(virtual3);

    fu.addOutputPortState(output1);
    
    TS_ASSERT_THROWS_NOTHING(executor1.addBinding(1, port1));
    TS_ASSERT_THROWS_NOTHING(executor1.addBinding(2, port2));
    TS_ASSERT_THROWS_NOTHING(executor1.addBinding(3, output1));

    TS_ASSERT_THROWS_NOTHING(executor2.addBinding(1, port1));
    TS_ASSERT_THROWS_NOTHING(executor2.addBinding(2, port2));
    TS_ASSERT_THROWS_NOTHING(executor2.addBinding(3, output1));

    TS_ASSERT_THROWS_NOTHING(executor3.addBinding(1, port1));
    TS_ASSERT_THROWS_NOTHING(executor3.addBinding(2, port2));
    TS_ASSERT_THROWS_NOTHING(executor3.addBinding(3, output1));

    fu.addOperationExecutor(executor1, add);
    fu.addOperationExecutor(executor2, sub);
    fu.addOperationExecutor(executor3, mul);

    // cycle one, ready in cycle 8
    SimValue value1(32);
    value1 = 2;
    port1.setValue(value1);
    SimValue value2(32);
    value2 = 2;
    virtual1.setValue(value2);

    SimValue output(32);
    output = 0;
    output1.setValue(output);

    fu.endClock();
    fu.advanceClock();

    // cycle two, ready in cycle 7
    TS_ASSERT_EQUALS(output1.value(), 0);

    value1 = 6;
    value2 = 3;
    port1.setValue(value1);
    virtual2.setValue(value2);
    
    fu.endClock();
    fu.advanceClock();
    
    // cycle three, ready in cycle 5
    TS_ASSERT_EQUALS(output1.value(), 0);
    
    value1 = 7;
    value2 = 6;
    port1.setValue(value1);
    virtual3.setValue(value2);
    
    fu.endClock();
    fu.advanceClock();
    
    // cycle four
    TS_ASSERT_EQUALS(output1.value(), 0);
    
    fu.endClock();
    fu.advanceClock();
    
    // cycle five
    TS_ASSERT_EQUALS(output1.value().intValue(), 42);
    
    fu.endClock();
    fu.advanceClock();
    
    // cycle six
    TS_ASSERT_EQUALS(output1.value().intValue(), 42);
    
    fu.endClock();
    fu.advanceClock();

    // cycle seven
    TS_ASSERT_EQUALS(output1.value().intValue(), 3);
    
    fu.endClock();
    fu.advanceClock();

    // cycle eight
    TS_ASSERT_EQUALS(output1.value().intValue(), 4);
}

/**
 * Test that MemoryAccessingFUState works.
 */
void
FUStateTest::testMemoryAccessingFUState() {

    OperationPool pool;
    Operation& load = pool.operation("TESTLOAD");
    Operation& store = pool.operation("TESTSTORE");

    IdealSRAM sram(0, 1000, 8, false);
    MemoryAccessingFUState fu(sram);
    OneCycleOperationExecutor loadExecutor(fu);
    OneCycleOperationExecutor storeExecutor(fu);

    InputPortState port1(fu, 32);
    TriggeringInputPortState port2(fu, 32);
    OpcodeSettingVirtualInputPortState virtual1(store, fu, port2);
    OpcodeSettingVirtualInputPortState virtual2(load, fu, port2);

    OutputPortState output(fu, 32);

    fu.addInputPortState(port1);
    fu.addInputPortState(port2);
    fu.addInputPortState(virtual1);
    fu.addInputPortState(virtual2);

    fu.addOutputPortState(output);

    TS_ASSERT_THROWS_NOTHING(loadExecutor.addBinding(1, port2));
    TS_ASSERT_THROWS_NOTHING(loadExecutor.addBinding(2, output));

    TS_ASSERT_THROWS_NOTHING(storeExecutor.addBinding(1, port1));
    TS_ASSERT_THROWS_NOTHING(storeExecutor.addBinding(2, port2));
    TS_ASSERT_THROWS_NOTHING(storeExecutor.addBinding(3, output));

    fu.addOperationExecutor(loadExecutor, load);
    fu.addOperationExecutor(storeExecutor, store);

    // first we save data
    SimValue value1(32);
    value1 = 100;
    SimValue value2(32);
    value2 = 10;
    port1.setValue(value1);
    virtual1.setValue(value2);
    
    fu.endClock();
    sram.advanceClock();
    fu.advanceClock();

    // then we load data.
    virtual2.setValue(value1);
    
    fu.endClock();
    sram.advanceClock();
    fu.advanceClock();

    TS_ASSERT_EQUALS(output.value().intValue(), 10);
}

#ifdef CONFLICT_DETECTOR_BENCHMARK 

#include <boost/timer.hpp>
#include <iostream>

#define INIT_COUNT 100000
#define EXEC_COUNT 10000000

#define BENCHMARK_INIT(FU_MACH, CREATE_DETECTOR) \
    std::cout.flush(); \
\
    t.restart(); \
    for (int count = 0; count < INIT_COUNT; ++count) {\
        FUConflictDetectorIndex detectors;\
        MachineStateBuilder builder;\
        FUResourceConflictDetector* detector = CREATE_DETECTOR;\
        if (detector != NULL) detectors[#FU_MACH] = detector;\
        MemorySystem memSys(srcMach);\
\
        delete builder.build(srcMach, memSys, detectors, false);\
        delete detector;\
    }\
    last = t.elapsed();\
    std::cout << last << "s ";\
    std::cout.flush()

#define BENCHMARK_INITIALIZATION(FU_MACH, CREATE_DETECTOR) {\
    std::cout \
        << "Initializing " #FU_MACH " " << INIT_COUNT << " times..."; \
    boost::timer t;\
    double best = DBL_MAX;\
    double last = 0.0;\
    const TTAMachine::Machine& srcMach = *FU_MACH##Mach;\
    const TTAMachine::FunctionUnit& fu = \
        *srcMach.functionUnitNavigator().item(0); \
\
    for (int i = 0; i < 3; ++i) {\
        BENCHMARK_INIT(FU_MACH, CREATE_DETECTOR);\
        best = std::min(best, last);\
    }\
\
    std::cout \
      << " best=" << best << " average=" << best/INIT_COUNT << std::endl;}


/**
 * Bechmark the different conflict detection models.
 *
 * These benchmarks will be reported in a paper.
 */
void
FUStateTest::benchmarkInitNone() {

    std::cout 
        << std::endl 
        << "### Benchmarking initialization of the conflict detection model "
        << "'none'" << std::endl;

    BENCHMARK_INITIALIZATION(mul, NULL);
    BENCHMARK_INITIALIZATION(alu, NULL);
    BENCHMARK_INITIALIZATION(fpu, NULL);
}

void
FUStateTest::benchmarkInitResourceVectors() {
    std::cout 
        << std::endl 
        << "### Benchmarking initialization of the conflict detection model "
        << "'resource vectors'" << std::endl;

#define CREATOR_RV new ResourceVectorFUResourceConflictDetector(fu)

    BENCHMARK_INITIALIZATION(mul, CREATOR_RV);
    BENCHMARK_INITIALIZATION(alu, CREATOR_RV);
    BENCHMARK_INITIALIZATION(fpu, CREATOR_RV);
}

void
FUStateTest::benchmarkInitActiveFSA() {
    std::cout 
        << std::endl 
        << "### Benchmarking initialization of the conflict detection model "
        << "'active FSA'" << std::endl;

#define CREATOR_AFSA new FSAFUResourceConflictDetector(fu, false)

    BENCHMARK_INITIALIZATION(mul, CREATOR_AFSA);
    BENCHMARK_INITIALIZATION(alu, CREATOR_AFSA);
    BENCHMARK_INITIALIZATION(fpu, CREATOR_AFSA);
}

void
FUStateTest::benchmarkInitLazyFSA() {
    std::cout 
        << std::endl 
        << "### Benchmarking initialization of the conflict detection model "
        << "'lazy FSA'" << std::endl;

#define CREATOR_LFSA new FSAFUResourceConflictDetector(fu, true)

    BENCHMARK_INITIALIZATION(mul, CREATOR_LFSA);
    BENCHMARK_INITIALIZATION(alu, CREATOR_LFSA);
    BENCHMARK_INITIALIZATION(fpu, CREATOR_LFSA);
}


#define BENCHMARK_SIMULATION(FU_MACH, CREATE_DETECTOR) \
    {\
        std::cout \
            << "Executing operations in " #FU_MACH " " << EXEC_COUNT << " times..."; \
        boost::timer t;\
        std::cout.flush();\
        double best = DBL_MAX;\
        double last = 0.0;\
        const TTAMachine::Machine& srcMach = *FU_MACH##Mach;\
        const TTAMachine::FunctionUnit& fu = \
           *srcMach.functionUnitNavigator().item(0); \
\
        for (int repeat = 0; repeat < 3; ++repeat) {\
            FUConflictDetectorIndex detectors;\
            MachineStateBuilder builder;\
            FUResourceConflictDetector* detector = CREATE_DETECTOR;\
            if (detector != NULL) detectors[#FU_MACH] = detector;\
            MemorySystem memSys(srcMach);\
\
            MachineState* msm = NULL;\
            CATCH_ANY(msm = builder.build(srcMach, memSys, detectors, false));\
            FUState& fuState = msm->fuState(#FU_MACH);\
\
            std::vector<ExecutableMove*> execMoves;\
            for (int i = 0; i < FU_MACH##Operations.size(); ++i) {\
                InlineImmediateValue* immediateSource = \
                    new InlineImmediateValue(1);\
               int immediate = 1;\
               SimValue val(immediate, 1);\
               immediateSource->setValue(val);\
\
               Operation* op = FU_MACH##Operations.at(i);\
               assert(op != NULL);\
               std::string portName = \
                   std::string("t.") + StringTools::stringToLower(op->name());\
               PortState& port = msm->portState(\
                   portName, #FU_MACH);\
               assert(&port != &NullPortState::instance());\
               execMoves.push_back(new BuslessExecutableMove(immediateSource, port));\
            }\
\
            t.restart();\
            for (int i = 0; i < EXEC_COUNT; ++i) {\
                execMoves.at(i % execMoves.size())->executeWrite();\
                fuState.endClock();\
                fuState.advanceClock();\
            }\
\
            last = t.elapsed();\
\
            std::cout << last << "s ";\
            std::cout.flush();\
\
            delete detector;\
            delete msm;\
            AssocTools::deleteAllItems(execMoves);\
\
            best = std::min(best, last);\
        }\
\
        std::cout \
            << " best=" << best << " average=" << best/EXEC_COUNT << std::endl;\
\
    }

void
FUStateTest::benchmarkSimulateNone() {

    std::cout 
        << std::endl 
        << "### Benchmarking simulation with the conflict detection model "
        << "'none'" << std::endl;

#define CREATOR_NONE NULL
    BENCHMARK_SIMULATION(mul, CREATOR_NONE);
    BENCHMARK_SIMULATION(alu, CREATOR_NONE);
    BENCHMARK_SIMULATION(fpu, CREATOR_NONE);
}

void
FUStateTest::benchmarkSimulateResourceVectors() {

    std::cout 
        << std::endl 
        << "### Benchmarking simulation with the conflict detection model "
        << "'resource vectors'" << std::endl;

    BENCHMARK_SIMULATION(mul, CREATOR_RV);
    BENCHMARK_SIMULATION(alu, CREATOR_RV);
    BENCHMARK_SIMULATION(fpu, CREATOR_RV);
}

void
FUStateTest::benchmarkSimulateActiveFSA() {

    std::cout 
        << std::endl 
        << "### Benchmarking simulation with the conflict detection model "
        << "'active FSA'" << std::endl;

    BENCHMARK_SIMULATION(mul, CREATOR_AFSA);
    BENCHMARK_SIMULATION(alu, CREATOR_AFSA);
    BENCHMARK_SIMULATION(fpu, CREATOR_AFSA);
}

void
FUStateTest::benchmarkSimulateLazyFSA() {

    std::cout 
        << std::endl 
        << "### Benchmarking simulation with the conflict detection model "
        << "'lazy FSA'" << std::endl;

    BENCHMARK_SIMULATION(mul, CREATOR_LFSA);
    BENCHMARK_SIMULATION(alu, CREATOR_LFSA);
    BENCHMARK_SIMULATION(fpu, CREATOR_LFSA);
}

#endif

/**
 * Run benchmarks for the different conflict detection models.
 */
void
FUStateTest::testConflictDetectionModelBenchmark() {
#ifdef CONFLICT_DETECTOR_BENCHMARK 

    benchmarkSimulateNone();
    benchmarkSimulateResourceVectors();
    benchmarkSimulateActiveFSA();
    benchmarkSimulateLazyFSA();

    benchmarkInitNone();
    benchmarkInitResourceVectors();
    benchmarkInitLazyFSA();
    benchmarkInitActiveFSA();
#endif
}

#endif
