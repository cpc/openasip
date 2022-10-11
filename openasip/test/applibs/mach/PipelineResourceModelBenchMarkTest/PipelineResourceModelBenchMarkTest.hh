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
 * @file PipelineResourceModelBenchMarkTest.hh
 *
 * A test suite for the different pipeline resource models.
 *
 * @author Pekka Jääskeläinen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef PIPELINE_RESOURCE_MODEL_BENCHMARK_TEST_HH
#define PIPELINE_RESOURCE_MODEL_BENCHMARK_TEST_HH

#include <TestSuite.h>
#include <string>
#include <fstream>
#include <boost/timer.hpp>

#include "Machine.hh"
#include "ReservationTableFUResourceConflictDetector.hh"
#include "DCMFUResourceConflictDetector.hh"
#include "FSAFUResourceConflictDetector.hh"
#include "ReservationTableFUResourceConflictDetector.hh"

class PipelineResourceModelBenchMarkTest : public CxxTest::TestSuite {
public:
    PipelineResourceModelBenchMarkTest();
    void testInitialization();
    void testSimulation();
private:
    const TTAMachine::FunctionUnit* ALU;
    const TTAMachine::FunctionUnit* mul;
    const TTAMachine::FunctionUnit* FPU;
};

//#define BENCHMARKING_ENABLED

PipelineResourceModelBenchMarkTest::PipelineResourceModelBenchMarkTest() :
    ALU(
        TTAMachine::Machine::loadFromADF(
            "data/alu.adf")->functionUnitNavigator().item("alu")),
    mul(
        TTAMachine::Machine::loadFromADF(
            "data/mul.adf")->functionUnitNavigator().item("mul")),
    FPU(
        TTAMachine::Machine::loadFromADF(
            "data/r4000.adf")->functionUnitNavigator().item("r4000")) {
}

#define INITIALIZATION_ROUNDS 3
#define INITIALIZATION_COUNT 1000

#define SIMULATION_ROUNDS 3
#define SIMULATED_OPERATION_COUNT (4000*100000)

#include <cfloat>

#define INIT_DETECTOR(DETECTOR__, FU__, CODE__) {       \
    double total = 0.0;\
    double best = DBL_MAX;\
    double worst = 0.0;\
    for (int round = 0; round < INITIALIZATION_ROUNDS; ++round) {\
        boost::timer t; \
        for (int i = 0; i < INITIALIZATION_COUNT; ++i) {\
            DETECTOR__ d(*FU__);                        \
            CODE__;\
        }\
        double val = t.elapsed();\
        Application::logStream() \
            << #FU__ << " round " << round << ": " << val << " s (" \
            << val / INITIALIZATION_COUNT << " on avg)" << std::endl;\
        if (val > worst)\
            worst = val;\
        if (val < best)\
            best = val;\
        total += val;\
    }\
\
    Application::logStream()\
        << "=== best: " << best << " worst: " << worst\
        << " average: " << total / INITIALIZATION_ROUNDS\
        << std::endl << std::endl;\
}

#define INIT(CLASS__, ADDITIONAL_CODE__) \
    Application::logStream() \
        << #CLASS__ << std::endl \
        << "--------------------------------------------------" << std::endl;\
    INIT_DETECTOR(CLASS__, mul, ADDITIONAL_CODE__)\
    INIT_DETECTOR(CLASS__, ALU, ADDITIONAL_CODE__)\
    INIT_DETECTOR(CLASS__, FPU, ADDITIONAL_CODE__)

/**
 * Runs the initialization benchmark.
 */
void
PipelineResourceModelBenchMarkTest::testInitialization() {
#if defined(BENCHMARKING_ENABLED) && defined(INIT_BENCHMARK)

    boost::timer t;
    Application::logStream() 
        << "INITIALIZATION_ROUNDS " << INITIALIZATION_ROUNDS << std::endl
        << "INITIALIZATION_COUNT " << INITIALIZATION_COUNT << std::endl;

    INIT(FSAFUResourceConflictDetector, d.initializeAllStates());
    INIT(FSAFUResourceConflictDetector, 0);
    INIT(DCMFUResourceConflictDetector, 0);
    INIT(ReservationTableFUResourceConflictDetector, 0);
#endif
}

#define SIMULATE_DETECTOR(DETECTOR__, FU__, CODE__, ISSUE__) {  \
    double total = 0.0;                                     \
    double best = DBL_MAX;\
    double worst = 0.0;\
    const int operations = FU__->operationCount();      \
    for (int round = 0; round < SIMULATION_ROUNDS; ++round) {\
        DETECTOR__ d(*FU__);                                 \
        CODE__;                                              \
        boost::timer t; \
        for (unsigned i = 0; i < SIMULATED_OPERATION_COUNT; ++i) {\
            const int op = i % (operations); \
            d.ISSUE__(op);\
            d.advanceCycleInline();\
        }                                                               \
        double val = t.elapsed();                                       \
        Application::logStream()                                        \
        << #FU__ << " round " << round << ": " << val << " s ("         \
        << SIMULATED_OPERATION_COUNT/(val*1e6) << " Mops)" << std::endl; \
        if (val > worst)\
            worst = val;\
        if (val < best)\
            best = val;\
        total += val;\
    }\
\
    Application::logStream()\
        << "=== best: " << best << " worst: " << worst\
        << " average: " << total / SIMULATION_ROUNDS\
        << std::endl << std::endl;\
}

#define SIMULATE(CLASS__, ADDITIONAL_CODE__, ISSUE__)   \
    Application::logStream() \
    << "simulate: " << #CLASS__ << std::endl                            \
    << "--------------------------------------------------" << std::endl; \
    SIMULATE_DETECTOR(CLASS__, mul, ADDITIONAL_CODE__, ISSUE__)         \
    SIMULATE_DETECTOR(CLASS__, ALU, ADDITIONAL_CODE__, ISSUE__)         \
    SIMULATE_DETECTOR(CLASS__, FPU, ADDITIONAL_CODE__, ISSUE__)         



/**
 * Runs the operation simulation benchmark.
 */
void
PipelineResourceModelBenchMarkTest::testSimulation() {
#ifdef BENCHMARKING_ENABLED

    Application::logStream()
        << "SIMULATION_ROUNDS" << SIMULATION_ROUNDS << std::endl
        << "SIMULATED_OPERATION_COUNT" << SIMULATED_OPERATION_COUNT 
        << std::endl;
    // active FSA
    SIMULATE(
        FSAFUResourceConflictDetector, d.initializeAllStates(), 
        issueOperationInline);
    // lazy FSA
    SIMULATE(FSAFUResourceConflictDetector, 0, issueOperationLazyInline);

    // DCM
    SIMULATE(DCMFUResourceConflictDetector, 0, issueOperationInline);

    // CRT
    SIMULATE(
        ReservationTableFUResourceConflictDetector, 0, 
        issueOperationInline);
#endif
}

#endif

