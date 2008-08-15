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
 * @file RegisterCopyAdderPassTest.hh
 *
 * A test suite for the basic block selector.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
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

class RegisterCopyAdderPassTest : public CxxTest::TestSuite {
public:
    RegisterCopyAdderPassTest();

    void setUp();
    void tearDown();

    void testPass();
    
private:
    int countOfGuardedMoves(const TTAProgram::Program& prog);
    /// Universal Machine for the unscheduled part.
    UniversalMachine umach_;
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
        TTAProgram::Program::loadFromTPEF(
            "data/arrmul_reg_allocated.tpef",
            *targetMachine, umach_));

    const int ORIGINAL_GUARD_COUNT = countOfGuardedMoves(*srcProgram);
#if 1
    for (int j = 0; j < srcProgram->procedureCount(); j++) {
        InterPassData interPassData;

        TTAProgram::Procedure& procedure = srcProgram->procedure(j);
        SimpleGuardAllocatorCore::allocateGuards(procedure, *targetMachine, interPassData);

        ControlFlowGraph cfg(procedure);

        for (int i = 0; i < cfg.nodeCount(); ++i) {
            BasicBlock& bb0 = 
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
                    
                    const std::string disassembly = bb0.disassemble();
#ifdef DEBUG_OUTPUT
                     Application::logStream() 
                        << "#### AFTER SCHEDULING: " << std::endl 
                        << disassembly << std::endl;
#endif                        

                    TS_ASSERT(
                        disassembly.find("i1.0 -> float1.15") == 
                        std::string::npos);
                    TS_ASSERT(
                        disassembly.find("i1.0 -> fu15.o0") != 
                        std::string::npos);
                    TS_ASSERT(
                        disassembly.find("i1.0 -> fu15.trigger.add") !=
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
