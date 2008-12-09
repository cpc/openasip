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
 * @file DisassemblyInstructionTest.hh 
 *
 * A test suite for disassembler instructions.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef DISASSEMBY_INSTRUCTION_TEST_HH
#define DISASSEMBY_INSTRUCTION_TEST_HH

#include <string>
#include <TestSuite.h>

#include "DisassemblyMove.hh"
#include "DisassemblyFPRegister.hh"
#include "DisassemblyIntRegister.hh"
#include "DisassemblyOperand.hh"
#include "DisassemblyFUPort.hh"
#include "DisassemblyFUOpcodePort.hh"
#include "DisassemblyRegister.hh"
#include "DisassemblyImmediate.hh"
#include "DisassemblyGuard.hh"
#include "DisassemblyInstruction.hh"
#include "DisassemblyImmediateRegister.hh"
#include "DisassemblyImmediateAssignment.hh"

using std::string;

class DisassemblyInstructionTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testMoves();
    void testDisassembly();
    void testInvalidMoveIndex();
};


/**
 * Called before each test.
 */
void
DisassemblyInstructionTest::setUp() {
}


/**
 * Called after each test.
 */
void
DisassemblyInstructionTest::tearDown() {
}


/**
 * Tests the instruction move indexing.
 */
void
DisassemblyInstructionTest::testMoves() {

    DisassemblyInstruction instruction;
    TS_ASSERT_EQUALS(static_cast<int>(instruction.moveCount()), 0);

    DisassemblyImmediateRegister* imm =
	new DisassemblyImmediateRegister("imm", 0);

    SimValue immValue(32);
    immValue = 42;

    DisassemblyRegister* register1 = new DisassemblyRegister("rf1", 1);
    DisassemblyRegister* register2 = new DisassemblyRegister("rf1", 2);
    DisassemblyRegister* register3 = new DisassemblyRegister("rf1", 3);

    DisassemblyFUPort* fuPort1 = new DisassemblyFUPort("add", "port1");
    DisassemblyFUPort* fuPort2 = new DisassemblyFUPort("add", "port2");

    DisassemblyGuard* guard = new DisassemblyGuard(register3, true);

    DisassemblyMove* move1 = new DisassemblyMove(register1, fuPort1);
    DisassemblyMove* move2 = new DisassemblyMove(register2, fuPort2, guard);

    DisassemblyImmediateAssignment* assignment =
	new DisassemblyImmediateAssignment(immValue, imm);


    instruction.addMove(move1);
    TS_ASSERT_EQUALS(static_cast<int>(instruction.moveCount()), 1);
    TS_ASSERT_EQUALS(&instruction.move(0), move1);

    instruction.addMove(move2);
    TS_ASSERT_EQUALS(static_cast<int>(instruction.moveCount()), 2);
    TS_ASSERT_EQUALS(&instruction.move(0), move1);
    TS_ASSERT_EQUALS(&instruction.move(1), move2);

    instruction.addMove(assignment);
    TS_ASSERT_EQUALS(static_cast<int>(instruction.moveCount()), 3);
    TS_ASSERT_EQUALS(&instruction.move(0), move1);
    TS_ASSERT_EQUALS(&instruction.move(1), move2);
    TS_ASSERT_EQUALS(&instruction.move(2), assignment);
}


/**
 * Tests disassembly of isntructions.
 */
void
DisassemblyInstructionTest::testDisassembly() {

    DisassemblyInstruction instruction;
    TS_ASSERT_EQUALS(instruction.toString(), " ;");

    DisassemblyImmediateRegister* imm =
	new DisassemblyImmediateRegister("imm", 0);

    SimValue immValue(32);
    immValue = 42;

    DisassemblyRegister* register1 = new DisassemblyRegister("rf1", 1);
    DisassemblyRegister* register2 = new DisassemblyRegister("rf2", 2);
    DisassemblyRegister* register3 = new DisassemblyRegister("rf3", 3);

    DisassemblyFUPort* fuPort1 = new DisassemblyFUPort("add", "port1");
    DisassemblyFUPort* fuPort2 = new DisassemblyFUPort("foo", "port2");

    DisassemblyGuard* guard = new DisassemblyGuard(register3, true);

    DisassemblyMove* move1 = new DisassemblyMove(register1, fuPort1);
    DisassemblyMove* move2 = new DisassemblyMove(register2, fuPort2, guard);

    DisassemblyImmediateAssignment* assignment =
	new DisassemblyImmediateAssignment(immValue, imm);

    instruction.addMove(move1);
    TS_ASSERT_EQUALS(instruction.toString(), "rf1.1 -> add.port1 ;");

    instruction.addMove(move2);
    string expected = "rf1.1 -> add.port1, !rf3.3 rf2.2 -> foo.port2 ;";
    TS_ASSERT_EQUALS(instruction.toString(), expected);

    instruction.addMove(assignment);
    expected = "rf1.1 -> add.port1, !rf3.3 rf2.2 -> foo.port2, [imm.0=42] ;";

    TS_ASSERT_EQUALS(instruction.toString(), expected);
}


/**
 * Tests invalid indexing of the instruction moves.
 */
void
DisassemblyInstructionTest::testInvalidMoveIndex() {

    DisassemblyInstruction instruction;

    // Test indexing of empty instruction.
    TS_ASSERT_THROWS_ANYTHING(instruction.move(0));
    TS_ASSERT_THROWS_ANYTHING(instruction.move(1));

    DisassemblyRegister* register1 = new DisassemblyRegister("rf1", 1);
    DisassemblyRegister* register2 = new DisassemblyRegister("rf1", 2);
    DisassemblyRegister* register3 = new DisassemblyRegister("rf1", 3);

    DisassemblyFUPort* fuPort1 = new DisassemblyFUPort("add", "port1");
    DisassemblyFUPort* fuPort2 = new DisassemblyFUPort("add", "port2");

    DisassemblyGuard* guard = new DisassemblyGuard(register3, true);

    DisassemblyMove* move1 = new DisassemblyMove(register1, fuPort1);
    DisassemblyMove* move2 = new DisassemblyMove(register2, fuPort2, guard);

    instruction.addMove(move1);

    TS_ASSERT_THROWS_ANYTHING(instruction.move(1));

    instruction.addMove(move2);

    TS_ASSERT_THROWS_ANYTHING(instruction.move(2));
}

#endif
