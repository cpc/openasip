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
 * @file RegisterCopyAdderPassTest.hh
 *
 * A test suite for the basic block selector.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef FU_REGISTER_COPY_ADDER_PASS_TEST_HH
#define FU_REGISTER_COPY_ADDER_PASS_TEST_HH

#include <TestSuite.h>
#include "BasicBlockScheduler.hh"
#include "OperationPool.hh"
#include "UniversalMachine.hh"
#include "BinaryStream.hh"
#include "BinaryReader.hh"
#include "Program.hh"
#include "Application.hh"
#include "POMDisassembler.hh"
#include "SimpleGuardAllocatorCore.hh"
#include "Procedure.hh"
#include "ControlFlowGraph.hh"
#include "RegisterCopyAdder.hh"
#include "BasicBlockScheduler.hh"
#include "InterPassData.hh"
#include "Instruction.hh"
#include "BasicBlock.hh"

class RegisterCopyAdderPassTest : public CxxTest::TestSuite {
public:
    RegisterCopyAdderPassTest();

    void setUp();
    void tearDown();

    void testPass();
    
private:
    int countOfGuardedMoves(const TTAProgram::Program& prog);
};

RegisterCopyAdderPassTest::RegisterCopyAdderPassTest() {
}

void 
RegisterCopyAdderPassTest::setUp() {
}

void 
RegisterCopyAdderPassTest::tearDown() {
}


/**
 * Returns the total count of guarded moves in the given program.
 *
 * @param prog The program to check.
 */
int
RegisterCopyAdderPassTest::countOfGuardedMoves(
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

//#define DEBUG_OUTPUT

/**
 * Tests that the copies are inserted.
 */
void
RegisterCopyAdderPassTest::testPass() {

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
            "data/arrmul_reg_allocated.tpef",
            *targetMachine));

    const int ORIGINAL_GUARD_COUNT = countOfGuardedMoves(*srcProgram);
#if 1
    for (int j = 0; j < srcProgram->procedureCount(); j++) {
        InterPassData interPassData;

        TTAProgram::Procedure& procedure = srcProgram->procedure(j);
        SimpleGuardAllocatorCore::allocateGuards(procedure, *targetMachine, interPassData);

        ControlFlowGraph cfg(procedure);

        for (int i = 0; i < cfg.nodeCount(); ++i) {
            TTAProgram::BasicBlock& bb0 = 
                (dynamic_cast<BasicBlockNode&>(cfg.node(i))).basicBlock();
        
            try {
#ifdef DEBUG_OUTPUT
                Application::logStream() 
                    << "#### BEFORE: " << std::endl 
                    << bb0.disassemble() << std::endl;
#endif
                BasicBlockScheduler bbScheduler(interPassData);
                bbScheduler.handleBasicBlock(bb0, *targetMachine);

#ifdef DEBUG_OUTPUT
                Application::logStream() 
                    << "#### AFTER SCHEDULING: " << std::endl 
                    << bb0.disassemble() << std::endl;
#endif                
                if (i == 0 && j == 0) {
                    /* BB 0 of procedure 0 had:

                       i1.0 -> float1.15
                       float1.15 -> fu16.o0,

                       and

                       i1.0 -> float1.15
                       float1.15 -> fu16.trigger.add

                       Thus, useless register copies.

                       Let's ensure we don't have that anymore, thus there's
                       direct moves 

                       i1.0 -> fu16.o0

                       and

                       i1.0 -> fu16.trigger.add

                       Use a text search on the disassembly to test it.
                    */
                    
                    const std::string disassembly = bb0.disassembly();
#ifdef DEBUG_OUTPUT
                     Application::logStream() 
                        << "#### AFTER SCHEDULING: " << std::endl 
                        << disassembly << std::endl;
#endif                        

                    TS_ASSERT(
                        disassembly.find("i1.0 -> float1.15") == 
                        std::string::npos);
                    TS_ASSERT(
                        disassembly.find("i1.0 -> fu16.o0") != 
                        std::string::npos);
                    TS_ASSERT(
                        disassembly.find("i1.0 -> fu16.trigger.add") !=
                        std::string::npos);
                }
            } catch (const Exception& e) {
                Application::logStream() 
                    << "on BB (" << j << "," << i << ") "
                    << Exception::lastExceptionInfo() << std::endl;
                assert(false);
            }
        }
    }
#endif
    TS_ASSERT_EQUALS(ORIGINAL_GUARD_COUNT, countOfGuardedMoves(*srcProgram));

}

#endif
