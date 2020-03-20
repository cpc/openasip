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
 * @file InstructionTest.hh
 *
 * A test suite for Instruction.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef INSTRUCTION_TEST_HH
#define INSTRUCTION_TEST_HH

#include <TestSuite.h>
#include "Instruction.hh"
#include "TerminalRegister.hh"
#include "Move.hh"
#include "Immediate.hh"
#include "Exception.hh"
#include "TerminalImmediate.hh"
#include "RFPort.hh"
#include "Bus.hh"
#include "POMDisassembler.hh"

using namespace TTAMachine;
using namespace TTAProgram;

class InstructionTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testInstruction();

private:
};


/**
 * Called before each test.
 */
void
InstructionTest::setUp() {
}


/**
 * Called after each test.
 */
void
InstructionTest::tearDown() {
}

/**
 * Tests that Instruction works properly.
 */
void
InstructionTest::testInstruction() {

    Instruction ins;

    TS_ASSERT_EQUALS(ins.moveCount(), 0);
    TS_ASSERT_EQUALS(ins.immediateCount(), 0);
    TS_ASSERT_THROWS(ins.move(0), OutOfRange);
    TS_ASSERT_THROWS(ins.immediate(0), OutOfRange);

    RegisterFile* reg_file = new RegisterFile(
        "RF", 32, 32, 1, 1, 0, RegisterFile::NORMAL);

    ImmediateUnit* immu = new ImmediateUnit(
        "immu", 2, 32, 1, 1, Machine::SIGN);

    RFPort* port1 = new RFPort("P1", *reg_file);
    RFPort* port2 = new RFPort("P2", *reg_file);
    RFPort* port3 = new RFPort("P3", *immu);

    UIntWord index1 = 10;
    UIntWord index2 = 11;

    TerminalRegister* src_reg = new TerminalRegister(
        *port1, index1);
    TerminalRegister* dst_reg = new TerminalRegister(
        *port2, index2);

    TerminalRegister* src_reg2 = new TerminalRegister(
        *port1, index1);
    TerminalRegister* dst_reg2 = new TerminalRegister(
        *port2, index2);

    TerminalRegister* immTerm = new TerminalRegister(*port3, 0);
    TerminalRegister* immTerm2 = new TerminalRegister(*port3, 1);

    Bus moveBus("Bus1", 32, 32, Machine::ZERO);

    auto move1 = std::make_shared<Move>(src_reg, dst_reg, moveBus);
    auto move2 = std::make_shared<Move>(src_reg2, dst_reg2, moveBus);

    TS_ASSERT_THROWS_NOTHING(ins.addMove(move1));
    TS_ASSERT_THROWS(ins.addMove(move1), ObjectAlreadyExists);
    TS_ASSERT_THROWS_NOTHING(ins.addMove(move2));
    TS_ASSERT_EQUALS(ins.moveCount(), 2);

    TS_ASSERT_THROWS(ins.move(ins.moveCount()), OutOfRange);
    TS_ASSERT_THROWS(ins.move(-1), OutOfRange);
    TS_ASSERT_THROWS_NOTHING(ins.move(ins.moveCount()-1));
    TS_ASSERT_EQUALS(ins.movePtr(0), move1);

    Machine dummy_mach;
    InstructionTemplate ins_template("dummy", dummy_mach);
    SimValue imm_value = SimValue(1232, 32);

    auto imm1 = std::make_shared<Immediate>(
        new TerminalImmediate(imm_value), immTerm);

    auto imm2 = std::make_shared<Immediate>(
        new TerminalImmediate(imm_value), immTerm2);

    TS_ASSERT_THROWS_NOTHING(ins.addImmediate(imm1));
    TS_ASSERT_THROWS(ins.addImmediate(imm1), ObjectAlreadyExists);
    TS_ASSERT_THROWS_NOTHING(ins.addImmediate(imm2));
    TS_ASSERT_EQUALS(ins.immediateCount(), 2);

    TS_ASSERT_THROWS(ins.immediate(ins.immediateCount()), OutOfRange);
    TS_ASSERT_THROWS(ins.immediate(-1), OutOfRange);
    TS_ASSERT_THROWS_NOTHING(ins.immediate(ins.immediateCount()-1));
    TS_ASSERT_EQUALS(ins.immediatePtr(0), imm1);

    TS_ASSERT_EQUALS(
        POMDisassembler::disassemble(ins), 
        POMDisassembler::disassemble(*(ins.copy())));

    delete reg_file;
}

#endif
