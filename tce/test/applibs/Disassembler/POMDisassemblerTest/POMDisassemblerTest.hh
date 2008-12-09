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
 * @file POMDisassemblerTest.hh
 *
 * A test suite for program object model disassembler.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
