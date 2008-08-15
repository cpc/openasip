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
 * @file POMDisassemblerTest.hh
 *
 * A test suite for program object model disassembler.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef POM_DISASSEMBLER_TEST_HH
#define POM_DISASSEMBLER_TEST_HH

#include <string>
#include <vector>
#include <TestSuite.h>
#include "POMDisassembler.hh"
#include "DisassemblyInstruction.hh"
#include "POMGenMacros.hh"
#include "SequenceTools.hh"
#include "ADFSerializer.hh"

using std::string;
using namespace TTAMachine;
using namespace TTAProgram;

class POMDisassemblerTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testSequentialDisassembly();
    void testParallelDisassembly();
};


/**
 * Called before each test.
 */
void
POMDisassemblerTest::setUp() {
    
}


/**
 * Called after each test.
 */
void
POMDisassemblerTest::tearDown() {
}


/**
 * Tests instruction disassembly with an univeral machine.
 */
void
POMDisassemblerTest::testSequentialDisassembly() {

    OperationPool pool;
    UniversalMachine machine(pool);

    // Create a sequential program using POMGenMacros.
    CREATE_PROGRAM(sequential1, machine);
    CREATE_PROCEDURE(main);

    INSTRUCTION(MOVE(IMM32(0), BOOL()));
    INSTRUCTION(MOVE(IMM32(123456), IREG(1)));
    INSTRUCTION(MOVE(IREG(1), OPERAND(ADD, 1)));
    INSTRUCTION(MOVE(IREG(2), OPERAND(ADD, 2)));
    INSTRUCTION(MOVE(OPERAND(ADD, 3), IREG(2)));
    SET_LABEL(some_label);
    INSTRUCTION(MOVE(IMM32(42), IREG(4)));
    INSTRUCTION(
	GMOVE(IMM32(LABEL(some_label)),
	      CONTROL_OPERAND(JUMP, 1), BOOLGUARD()));

    // Disassemble the sequential code created with macros.
    POMDisassembler disassembler(sequential1);
    Word count = disassembler.instructionCount();

    TS_ASSERT_EQUALS(static_cast<int>(count), 7);

    std::vector<DisassemblyInstruction*> disassembly;

    for (unsigned int i = 0; i < count; i++) {
        disassembly.push_back(disassembler.createInstruction(i));
    }

    TS_ASSERT_EQUALS(disassembly[0]->toString(), "0 -> bool ;");
    TS_ASSERT_EQUALS(disassembly[1]->toString(), "123456 -> sp ;");
    TS_ASSERT_EQUALS(disassembly[2]->toString(), "sp -> add.1 ;");
    TS_ASSERT_EQUALS(disassembly[3]->toString(), "iarg1 -> add.2 ;");
    TS_ASSERT_EQUALS(disassembly[4]->toString(), "add.3 -> iarg1 ;");
    TS_ASSERT_EQUALS(disassembly[5]->toString(), "42 -> iarg3 ;");
    TS_ASSERT_EQUALS(disassembly[6]->toString(), "? 5 -> jump.1 ;");

    SequenceTools::deleteAllItems(disassembly);
}


/**
 * Tests instruction disassembly with parallel code.
 */
void
POMDisassemblerTest::testParallelDisassembly() {

    ADFSerializer reader;
    reader.setSourceFile("data/testmachine.adf");
    Machine* machine = reader.readMachine();
    const Machine& mach = *machine;

    CREATE_PROGRAM(parallel1, mach);
    CREATE_PROCEDURE(main);

    INSTRUCTION(
        MOVE(IMM32(99), REG(RF, 1))
        MOVE(IMM32(42), REG(RF, 2))
        NOP);

    INSTRUCTION(
        MOVE(REG(RF, 1), PORT(ALU, P1))
        MOVE(REG(RF, 2), OPORT(ALU, P2, MUL))
        NOP);

    INSTRUCTION(NOP NOP NOP);

    INSTRUCTION(
	MOVE(IMM32(99), REG(RF, 8))
	NOP
	NOP);

    INSTRUCTION(
	MOVE(IMM32(5), CONTROL_OPERAND(call, 1))
	NOP
	NOP);

    // Disassemble the parallel code created with macros.
    POMDisassembler disassembler(parallel1);
    Word count = disassembler.instructionCount();

    TS_ASSERT_EQUALS(static_cast<int>(count), 5);

    std::vector<DisassemblyInstruction*> disassembly;

    for (unsigned int i = 0; i < count; i++) {
        disassembly.push_back(disassembler.createInstruction(i));
    }

    TS_ASSERT_EQUALS(disassembly[0]->toString(), "99 -> RF.1, 42 -> RF.2, ... ;");
    TS_ASSERT_EQUALS(disassembly[1]->toString(),
		     "RF.1 -> ALU.P1, RF.2 -> ALU.P2.mul, ... ;");
    TS_ASSERT_EQUALS(disassembly[2]->toString(), "..., ..., ... ;");
    TS_ASSERT_EQUALS(disassembly[3]->toString(), "99 -> RF.8, ..., ... ;");
    TS_ASSERT_EQUALS(disassembly[4]->toString(), "5 -> control_unit.op.call, ..., ... ;");

    SequenceTools::deleteAllItems(disassembly);

    delete machine;
}

#endif
