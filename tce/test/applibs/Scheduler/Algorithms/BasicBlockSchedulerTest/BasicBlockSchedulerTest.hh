/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file BasicBlockSchedulerTest.hh
 *
 * A test suite for the basic block selector.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef FU_BASIC_BLOCK_SCHEDULER_TEST_HH
#define FU_BASIC_BLOCK_SCHEDULER_TEST_HH

#include <TestSuite.h>
#include "BBSchedulerController.hh"
#include "CycleLookBackSoftwareBypasser.hh"
#include "OperationPool.hh"
#include "UniversalMachine.hh"
#include "BinaryStream.hh"
#include "BinaryReader.hh"
#include "Program.hh"
#include "Application.hh"
#include "POMDisassembler.hh"
#include "SimpleGuardAllocatorCore.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "InterPassData.hh"
#include "Terminal.hh"
#include "GlobalScope.hh"


class BasicBlockSchedulerTest : public CxxTest::TestSuite {
public:
    BasicBlockSchedulerTest();

    void setUp();
    void tearDown();

    void testScheduleShortImm();
    void testSchedule();
    void testScheduleShortImmAndReducedConnectivity();
    void testScheduleWithDisabledDRE();

private:
    int countOfGuardedMoves(const TTAProgram::Program& prog);
};

BasicBlockSchedulerTest::BasicBlockSchedulerTest() {
}

void
BasicBlockSchedulerTest::setUp() {
}

void
BasicBlockSchedulerTest::tearDown() {
}

/**
 * Tests scheduling against a machine with very short immediate fields
 * (3-4 bit) and reduced connectivity (which should result in temp moves).
 */
void
BasicBlockSchedulerTest::testScheduleShortImmAndReducedConnectivity() {
    //return;
    /// The tested input program with registers allocated.
    TTAProgram::Program* srcProgram = NULL;
    /// Target machine to schedule the program for.
    TTAMachine::Machine* targetMachine = NULL;

    CATCH_ANY(
        targetMachine =
        TTAMachine::Machine::loadFromADF(
            "data/3_bus_short_immediate_fields_and_reduced_connectivity.adf"));

    CATCH_ANY(
        srcProgram =
        TTAProgram::Program::loadFromUnscheduledTPEF(
            "data/arrmul_reg_allocated_10_bus.tpef",
            *targetMachine));

    const int ORIGINAL_PROCEDURE_COUNT = srcProgram->procedureCount();
    const int ORIGINAL_LABEL_COUNT =
        srcProgram->globalScopeConst().globalCodeLabelCount();
    const int ORIGINAL_GUARD_COUNT = countOfGuardedMoves(*srcProgram);

    InterPassData interPassData;

    // allocate the guards before passing the program to the BB scheduler
    for (int j = 0; j < srcProgram->procedureCount(); j++) {
        TTAProgram::Procedure& procedure = srcProgram->procedure(j);
        SimpleGuardAllocatorCore::allocateGuards(
            procedure, *targetMachine, interPassData);
    }

    TS_ASSERT_EQUALS(
        ORIGINAL_GUARD_COUNT, countOfGuardedMoves(*srcProgram));

    BBSchedulerController scheduler(interPassData);

    try {
        scheduler.handleProgram(*srcProgram, *targetMachine);
    } catch (const Exception& e) {
        Application::logStream() << e.errorMessageStack() << std::endl;
        assert(false);
    }
    // two guarded moves split, thus create two new guarded moves
    // + 4 delay slots filled, total +6
    TS_ASSERT_EQUALS(
        ORIGINAL_GUARD_COUNT, countOfGuardedMoves(*srcProgram));

    TS_ASSERT_EQUALS(
        srcProgram->procedureCount(), ORIGINAL_PROCEDURE_COUNT);

    TS_ASSERT_EQUALS(
        srcProgram->globalScopeConst().globalCodeLabelCount(),
        ORIGINAL_LABEL_COUNT);

#if 0
    Application::logStream()
        << std::endl << "after:" << std::endl
        << POMDisassembler::disassemble(*srcProgram, true) << std::endl;
#endif

    TTAProgram::Program::writeToTPEF(
        *srcProgram, "data/out.par.tpef");

    delete srcProgram;
    srcProgram = NULL;

    // test that the written TPEF is loadable back to POM
    CATCH_ANY(
        delete TTAProgram::Program::loadFromUnscheduledTPEF(
            "data/out.par.tpef",
            *targetMachine));
}


/**
 * Tests scheduling against a machine with very short immediate fields (4 bit),
 * thus should produce long immediate moves.
 */
void
BasicBlockSchedulerTest::testScheduleShortImm() {
    //return;
    /// The tested input program with registers allocated.
    TTAProgram::Program* srcProgram = NULL;
    /// Target machine to schedule the program for.
    TTAMachine::Machine* targetMachine = NULL;

    CATCH_ANY(
        targetMachine =
        TTAMachine::Machine::loadFromADF(
            "data/3_bus_short_immediate_fields.adf"));

    CATCH_ANY(
        srcProgram =
        TTAProgram::Program::loadFromUnscheduledTPEF(
            "data/arrmul_reg_allocated_10_bus.tpef",
            *targetMachine));

    const int ORIGINAL_PROCEDURE_COUNT = srcProgram->procedureCount();
    const int ORIGINAL_LABEL_COUNT =
        srcProgram->globalScopeConst().globalCodeLabelCount();
    const int ORIGINAL_GUARD_COUNT = countOfGuardedMoves(*srcProgram);

    InterPassData interPassData;

    // allocate the guards before passing the program to the BB scheduler
    for (int j = 0; j < srcProgram->procedureCount(); j++) {
        TTAProgram::Procedure& procedure = srcProgram->procedure(j);
        SimpleGuardAllocatorCore::allocateGuards(
            procedure, *targetMachine, interPassData);
    }

    BBSchedulerController scheduler(interPassData);

    CATCH_ANY(
        scheduler.handleProgram(*srcProgram, *targetMachine));

    // two guarded moves split, thus create two new guarded moves
    // + 10 guarded moves filled
    TS_ASSERT_EQUALS(
        ORIGINAL_GUARD_COUNT, countOfGuardedMoves(*srcProgram));

    TS_ASSERT_EQUALS(
        srcProgram->procedureCount(), ORIGINAL_PROCEDURE_COUNT);

    TS_ASSERT_EQUALS(
        srcProgram->globalScopeConst().globalCodeLabelCount(),
        ORIGINAL_LABEL_COUNT);

#if 0
    Application::logStream()
        << std::endl << "after:" << std::endl
        << POMDisassembler::disassemble(*srcProgram, true) << std::endl;
#endif

    TTAProgram::Program::writeToTPEF(
        *srcProgram, "data/arrmul_reg_allocated_10_bus.par.tpef");

    delete srcProgram;
    srcProgram = NULL;

    // test that the written TPEF is loadable back to POM
    CATCH_ANY(
        delete TTAProgram::Program::loadFromUnscheduledTPEF(
            "data/arrmul_reg_allocated_10_bus.par.tpef",
            *targetMachine));
}

/**
 * Returns the total count of guarded moves in the given program.
 *
 * @param prog The program to check.
 */
int
BasicBlockSchedulerTest::countOfGuardedMoves(
    const TTAProgram::Program& prog) {

    int count = 0;

    for (int procIndex = 0; procIndex < prog.procedureCount(); procIndex++) {
        TTAProgram::Procedure& procedure = prog.procedure(procIndex);
        for (int i = 0; i < procedure.instructionCount(); ++i) {
            TTAProgram::Instruction& instruction =
                procedure.instructionAtIndex(i);
            for (int m = 0; m < instruction.moveCount(); ++m) {
                TTAProgram::Move& move = instruction.move(m);
                if (!move.isUnconditional())
                    ++count;
            }
        }
    }
    return count;
}

/**
 * Tests scheduling the simple test case.
 */
void
BasicBlockSchedulerTest::testSchedule() {
    //return;
    /// The tested input program with always-write-back-results.
    TTAProgram::Program* srcProgram = NULL;
    /// Compare with always-write-back-results disabled.
    TTAProgram::Program* srcProgram2 = NULL;
    /// Target machine to schedule the program for with
    /// always-write-back-results set.
    TTAMachine::Machine* targetMachine = NULL;
    /// Same target machine without the always-write-back-results set.
    TTAMachine::Machine* targetMachine2 = NULL;

    CATCH_ANY(
        targetMachine =
        TTAMachine::Machine::loadFromADF(
            "data/10_bus_full_connectivityNoDRE.adf"));
    CATCH_ANY(
        targetMachine2 =
        TTAMachine::Machine::loadFromADF(
            "data/10_bus_full_connectivity.adf"));

    CATCH_ANY(
        srcProgram =
        TTAProgram::Program::loadFromUnscheduledTPEF(
            "data/arrmul_reg_allocated_10_bus.tpef",
            *targetMachine));
    CATCH_ANY(
        srcProgram2 =
        TTAProgram::Program::loadFromUnscheduledTPEF(
            "data/arrmul_reg_allocated_10_bus.tpef",
            *targetMachine2));

    InterPassData interPassData;

    // allocate the guards before passing the program to the BB scheduler
    for (int j = 0; j < srcProgram->procedureCount(); j++) {
        TTAProgram::Procedure& procedure = srcProgram->procedure(j);
        SimpleGuardAllocatorCore::allocateGuards(
            procedure, *targetMachine, interPassData);
    }
    for (int j = 0; j < srcProgram2->procedureCount(); j++) {
        TTAProgram::Procedure& procedure = srcProgram2->procedure(j);
        SimpleGuardAllocatorCore::allocateGuards(
            procedure, *targetMachine2, interPassData);
    }

    CycleLookBackSoftwareBypasser bypasser;
    BBSchedulerController scheduler(interPassData, &bypasser);
    CATCH_ANY(
        scheduler.handleProgram(*srcProgram, *targetMachine));
    TS_ASSERT(targetMachine->alwaysWriteResults());
    TTAProgram::Procedure& procedure = srcProgram->procedure(0);
    // targetMachine has always-write-back-results enabled
    // so there should be 3 writes to GPR in this procedure
    int resultWrites = 0;
    TTAProgram::Instruction* ins = NULL;
    for (int j = 0; j < procedure.instructionCount(); j++) {
        ins = &procedure.instructionAt(j);
        for (int i = 0; i < ins->moveCount(); i++) {
            if (ins->move(i).destination().isGPR())
                resultWrites++;
        }
    }

    // test same program on same machine without the 
    // always-write-back-results set
    CycleLookBackSoftwareBypasser bypasser2;
    BBSchedulerController scheduler2(interPassData, &bypasser2);
    CATCH_ANY(
        scheduler2.handleProgram(*srcProgram2, *targetMachine2));
    TS_ASSERT(!targetMachine2->alwaysWriteResults());
    TTAProgram::Procedure& procedure2 = srcProgram2->procedure(0);
    // DRE should remove the result writes found in previous case
    // Only one should remain
    int resultWrites2 = 0;
    for (int j = 0; j < procedure2.instructionCount(); j++) {
        ins = &procedure2.instructionAt(j);
        for (int i = 0; i < ins->moveCount(); i++) {
            if (ins->move(i).destination().isGPR())
                resultWrites2++;
        }
    }
    // Without the always-write-back-results there DRE should
    // remove number of register writes (2 to be exact).
    TS_ASSERT(resultWrites > resultWrites2);
#if 0
    Application::logStream()
        << std::endl << "after:" << std::endl
        << POMDisassembler::disassemble(procedure, true) << std::endl;
    Application::logStream()
        << std::endl << "after:" << std::endl
        << POMDisassembler::disassemble(procedure2, true) << std::endl;
#endif

    TTAProgram::Program::writeToTPEF(
        *srcProgram, "data/arrmul_reg_allocated_10_bus.par.tpef");

    delete srcProgram;
    srcProgram = NULL;

    // test that the written TPEF is loadable back to POM
    CATCH_ANY(
        delete TTAProgram::Program::loadFromUnscheduledTPEF(
            "data/arrmul_reg_allocated_10_bus.par.tpef",
            *targetMachine));
}

/**
 * Tests scheduling the simple test case.
 */
void
BasicBlockSchedulerTest::testScheduleWithDisabledDRE() {
}



#endif
