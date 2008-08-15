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
 * @file DisassemblyInstructionTest.hh 
 *
 * A test suite for disassembler instructions.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
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
