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
 * @file DisassemblyMoveTest.hh 
 *
 * A test suite for disassembler moves.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef DISASSEMBY_MOVE_TEST_HH
#define DISASSEMBY_MOVE_TEST_HH

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
#include "DisassemblyBoolRegister.hh"
#include "DisassemblyImmediateRegister.hh"
#include "DisassemblyControlUnitPort.hh"
#include "DisassemblyImmediateAssignment.hh"
#include "SimValue.hh"

using std::string;

class DisassemblyMoveTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testUnguardedMove();
    void testGuardedMove();
    void testImmediateAssignment();
};


/**
 * Called before each test.
 */
void
DisassemblyMoveTest::setUp() {
}


/**
 * Called after each test.
 */
void
DisassemblyMoveTest::tearDown() {
}


/**
 * Tests unguarded move disassembly with different source and destination
 * elements.
 */
void
DisassemblyMoveTest::testUnguardedMove() {

    SimValue immValue1(32);
    SimValue immValue2(32);
    immValue1 = 87654321;
    immValue2 = -123;

    // Create two instances of all DisassemblyElements.
    DisassemblyFPRegister* fpRegister1 = new DisassemblyFPRegister(0);
    DisassemblyFPRegister* fpRegister2 = new DisassemblyFPRegister(321);
    DisassemblyIntRegister* intRegister1 = new DisassemblyIntRegister(456);
    DisassemblyIntRegister* intRegister2 = new DisassemblyIntRegister(1);
    DisassemblyIntRegister* intRegister3 = new DisassemblyIntRegister(10);
    DisassemblyIntRegister* intRegister4 = new DisassemblyIntRegister(0);
    DisassemblyFUPort* fuPort1 = new DisassemblyFUPort("foounit", "port4");
    DisassemblyFUPort* fuPort2 = new DisassemblyFUPort("barunit", "port1");
    DisassemblyFUPort* fuPort3 = new DisassemblyFUPort("barunit", "port10");
    DisassemblyImmediate* immediate1 = new DisassemblyImmediate(immValue1, true);
    DisassemblyImmediate* immediate2 = new DisassemblyImmediate(immValue2, true);
    DisassemblyBoolRegister* boolReg = new DisassemblyBoolRegister();
    DisassemblyImmediateRegister* immReg =
	new DisassemblyImmediateRegister("imm", 12345);

    DisassemblyControlUnitPort* gcuPort = new DisassemblyControlUnitPort("ra");

    DisassemblyOperand* operand1 = new DisassemblyOperand("someoperation", 42);
    DisassemblyOperand* operand2 =
	new DisassemblyOperand("anotheroperation", 1);

    DisassemblyRegister* register1 = new DisassemblyRegister("rf3", 123);
    DisassemblyRegister* register2 = new DisassemblyRegister("rf9", 0);

    DisassemblyFUOpcodePort* fuOpcodePort1 =
	new DisassemblyFUOpcodePort("alu", "port3", "add");
    DisassemblyFUOpcodePort* fuOpcodePort2 =
	new DisassemblyFUOpcodePort("magic", "port1", "conjure");


    // immediate -> FU port
    DisassemblyMove move01(immediate1, fuPort1);
    string move01str = move01.toString();
    TS_ASSERT_EQUALS(move01str, "87654321 -> foounit.port4");

    // FU port -> operand
    DisassemblyMove move02(fuPort2, operand1);
    string move02str = move02.toString();
    TS_ASSERT_EQUALS(move02str, "barunit.port1 -> someoperation.42");

    // int register -> fu opcode port
    DisassemblyMove move03(intRegister1, fuOpcodePort1);
    string move03str = move03.toString();
    TS_ASSERT_EQUALS(move03str, "r456 -> alu.port3.add");

    // immediate -> register
    DisassemblyMove move04(immediate2, register1);
    string move04str = move04.toString();
    TS_ASSERT_EQUALS(move04str, "-123 -> rf3.123");

    // float register (return value) -> operand
    DisassemblyMove move05(fpRegister1, operand2);
    string move05str = move05.toString();
    TS_ASSERT_EQUALS(move05str, "fres0 -> anotheroperation.1");

    // int register (stack pointer) -> opcodeport
    DisassemblyMove move06(intRegister2, fuOpcodePort2);
    string move06str = move06.toString();
    TS_ASSERT_EQUALS(move06str, "sp -> magic.port1.conjure");

    // register -> float register
    DisassemblyMove move07(register2, fpRegister2);
    string move07str = move07.toString();
    TS_ASSERT_EQUALS(move07str, "rf9.0 -> f321");

    // immediate register -> bool register
    DisassemblyMove move08(immReg, boolReg);
    string move08str = move08.toString();
    TS_ASSERT_EQUALS(move08str, "imm.12345 -> bool");

    // control unit port -> int register
    DisassemblyMove move09(gcuPort, intRegister3);
    string move09str = move09.toString();
    TS_ASSERT_EQUALS(move09str, "ra -> r10");

    // int register (return value) -> fu port
    DisassemblyMove move10(intRegister4, fuPort3);
    string move10str = move10.toString();
    TS_ASSERT_EQUALS(move10str, "ires0 -> barunit.port10");
}


/**
 * Tests guarded move disassembly.
 */
void
DisassemblyMoveTest::testGuardedMove() {

    SimValue immValue(32);
    immValue = 1234;

    // Create move elements.
    DisassemblyImmediate* immediate = new DisassemblyImmediate(immValue, true);

    DisassemblyRegister* guardedRegister1 = new DisassemblyRegister("rf4", 1);
    DisassemblyRegister* guardedRegister2 = new DisassemblyRegister("rf4", 2);
    DisassemblyRegister* register1 = new DisassemblyRegister("rf", 9);

    DisassemblyFUPort* fuPort = new DisassemblyFUPort("unit", "port");
    DisassemblyFUOpcodePort* fuOpcodePort =
	new DisassemblyFUOpcodePort("magic", "port1", "conjure");

    // Create guards.
    DisassemblyGuard* guard1 = new DisassemblyGuard(guardedRegister1, false);
    DisassemblyGuard* guard2 = new DisassemblyGuard(guardedRegister2, true);

    // Create and test guarded moves.
    DisassemblyMove move1(immediate, fuPort, guard1);
    string move1str = move1.toString();
    TS_ASSERT_EQUALS(move1str, "?rf4.1 1234 -> unit.port");

    DisassemblyMove move2(register1, fuOpcodePort, guard2);
    string move2str = move2.toString();
    TS_ASSERT_EQUALS(move2str, "!rf4.2 rf.9 -> magic.port1.conjure");
}

/**
 * Tests immediate assignment disassembly.
 */
void
DisassemblyMoveTest::testImmediateAssignment() {
    DisassemblyImmediateRegister* imm =
        new DisassemblyImmediateRegister("imm", 0);
    SimValue value(32);
    value = 123;
    DisassemblyImmediateAssignment immAssignment(value, imm);
    string immAssignmentStr = immAssignment.toString();
    TS_ASSERT_EQUALS(immAssignmentStr, "[imm.0=123]");
}

#endif
