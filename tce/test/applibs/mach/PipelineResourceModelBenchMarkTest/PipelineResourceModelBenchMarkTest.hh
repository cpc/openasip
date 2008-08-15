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
 * @file PipelineResourceModelBenchMarkTest.hh
 *
 * A test suite for the different pipeline resource models.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
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

