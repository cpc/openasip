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
 * @file TerminalTest.hh
 * 
 * A test suite for Terminal.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TERMINAL_TEST_HH
#define TERMINAL_TEST_HH

#include <TestSuite.h>
#include "Terminal.hh"
#include "TerminalImmediate.hh"
#include "TerminalAddress.hh"
#include "TerminalInstructionAddress.hh"
#include "TerminalRegister.hh"
#include "TerminalFUPort.hh"
#include "Operation.hh"
#include "HWOperation.hh"
#include "RegisterFile.hh"
#include "Machine.hh"
#include "FUPort.hh"
#include "TCEString.hh"

using namespace TTAMachine;
using namespace TTAProgram;

class TerminalTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testTerminalImmediate();
    void testTerminalAddress();
    void testTerminalInstructionAddress();
    void testTerminalRegister();
    void testTerminalFUPort();

private:
};


/**
 * Called before each test.
 */
void
TerminalTest::setUp() {
}


/**
 * Called after each test.
 */
void
TerminalTest::tearDown() {
}

/**
 * Tests that TerminalImmediate works as defined.
 */
void
TerminalTest::testTerminalImmediate() {

    SimValue imm = SimValue(129, 32);
    TerminalImmediate terminal_imm(imm);

    TS_ASSERT_EQUALS(terminal_imm.isImmediate(), true);
    TS_ASSERT_EQUALS(terminal_imm.isImmediateRegister(), false);
    TS_ASSERT_EQUALS(terminal_imm.isGPR(), false);
    TS_ASSERT_EQUALS(terminal_imm.isFUPort(), false);

    TS_ASSERT_EQUALS(terminal_imm.value(), imm);
    TS_ASSERT_THROWS(terminal_imm.registerFile(), WrongSubclass);
    TS_ASSERT_THROWS(terminal_imm.immediateUnit(), WrongSubclass);
    TS_ASSERT_THROWS(terminal_imm.functionUnit(), WrongSubclass);

    TS_ASSERT_THROWS(terminal_imm.index(), WrongSubclass);
    TS_ASSERT_THROWS(terminal_imm.operation(), WrongSubclass);
    TS_ASSERT_THROWS(terminal_imm.port(), WrongSubclass);

    UIntWord index = 12;
    TS_ASSERT_THROWS(terminal_imm.setIndex(index), WrongSubclass);

}

/**
 * Tests that TerminalAddress works as defined.
 */
void
TerminalTest::testTerminalAddress() {

    Machine dummy_machine;
    AddressSpace as("AS", 16, 0, 1000, dummy_machine);
    SimValue loc = SimValue(0xFF, 32);

    TerminalAddress terminal_addr(loc, as);
}

/**
 * Tests that TerminalInstructionAddress works as defined.
 */
void
TerminalTest::testTerminalInstructionAddress() {
}

/**
 * Tests that TerminalRegister works as defined.
 */
void
TerminalTest::testTerminalRegister() {

    RegisterFile* reg_file = new RegisterFile(
        "RF", 32, 32, 1, 1, 0, RegisterFile::NORMAL);
    RFPort* port1 = new RFPort("P1", *reg_file);
    SIntWord index1 = 10;
    TerminalRegister terminal_reg(*port1, index1);

    TS_ASSERT_EQUALS(terminal_reg.isImmediate(), false);
    TS_ASSERT_EQUALS(terminal_reg.isImmediateRegister(), false);
    TS_ASSERT_EQUALS(terminal_reg.isGPR(), true);
    TS_ASSERT_EQUALS(terminal_reg.isFUPort(), false);

    TS_ASSERT_THROWS(terminal_reg.value(), WrongSubclass);
    TS_ASSERT_EQUALS(terminal_reg.registerFile().name(), reg_file->name());
    TS_ASSERT_THROWS(terminal_reg.immediateUnit(), WrongSubclass);
    TS_ASSERT_THROWS(terminal_reg.functionUnit(), WrongSubclass);

    TS_ASSERT_EQUALS(terminal_reg.index(), index1);
    TS_ASSERT_THROWS(terminal_reg.operation(), WrongSubclass);
    TS_ASSERT_EQUALS(terminal_reg.port().name(), port1->name());

    UIntWord tooLargeIndex = reg_file->numberOfRegisters();
    SIntWord goodIndex = reg_file->numberOfRegisters() - 1;
    TS_ASSERT_THROWS(terminal_reg.setIndex(tooLargeIndex), OutOfRange);
    TS_ASSERT_EQUALS(terminal_reg.index(), index1);
    TS_ASSERT_THROWS_NOTHING(terminal_reg.setIndex(goodIndex));
    TS_ASSERT_EQUALS(terminal_reg.index(), goodIndex);

    ImmediateUnit* imm = new ImmediateUnit("IMM", 16, 16, 1, 0, Machine::ZERO);
    RFPort* port2 = new RFPort("P2", *imm);
    SIntWord index2 = 2;
    TerminalRegister terminal_imm(*port2, index2);

    TS_ASSERT_EQUALS(terminal_imm.isImmediate(), false);
    TS_ASSERT_EQUALS(terminal_imm.isImmediateRegister(), true);
    TS_ASSERT_EQUALS(terminal_imm.isGPR(), false);
    TS_ASSERT_EQUALS(terminal_imm.isFUPort(), false);

    TS_ASSERT_THROWS(terminal_imm.value(), WrongSubclass);
    TS_ASSERT_THROWS(terminal_imm.registerFile(), WrongSubclass);
    TS_ASSERT_EQUALS(terminal_imm.immediateUnit().name(), imm->name());
    TS_ASSERT_THROWS(terminal_imm.functionUnit(), WrongSubclass);

    TS_ASSERT_EQUALS(terminal_imm.index(), index2);
    TS_ASSERT_THROWS(terminal_imm.operation(), WrongSubclass);
    TS_ASSERT_EQUALS(terminal_imm.port().name(), port2->name());

    tooLargeIndex = imm->numberOfRegisters();
    goodIndex = imm->numberOfRegisters() - 1;
    TS_ASSERT_THROWS(terminal_imm.setIndex(tooLargeIndex), OutOfRange);
    TS_ASSERT_EQUALS(terminal_imm.index(), index2);
    TS_ASSERT_THROWS_NOTHING(terminal_imm.setIndex(goodIndex));
    TS_ASSERT_EQUALS(terminal_imm.index(), goodIndex);

    delete reg_file;
    delete imm;
}

/**
 * Tests that TerminalFUPort works as defined.
 */
void
TerminalTest::testTerminalFUPort() {

    FunctionUnit* fu = new FunctionUnit("FU");
    FUPort* port = new FUPort("P1", 32, *fu, true, true);

    const std::string OP_NAME = "ADD";
    HWOperation* hw_op = new HWOperation(OP_NAME, *fu);
    hw_op->bindPort(1, *port);    
	
    TerminalFUPort terminal_port(*hw_op, 1);

    TS_ASSERT_EQUALS(terminal_port.isImmediate(), false);
    TS_ASSERT_EQUALS(terminal_port.isImmediateRegister(), false);
    TS_ASSERT_EQUALS(terminal_port.isGPR(), false);
    TS_ASSERT_EQUALS(terminal_port.isFUPort(), true);

    TS_ASSERT_THROWS(terminal_port.value(), WrongSubclass);
    TS_ASSERT_THROWS(terminal_port.registerFile(), WrongSubclass);
    TS_ASSERT_THROWS(terminal_port.immediateUnit(), WrongSubclass);
    TS_ASSERT_EQUALS(terminal_port.functionUnit().name(), fu->name());

    TS_ASSERT_THROWS(terminal_port.index(), WrongSubclass);
    TS_ASSERT_EQUALS(terminal_port.port().name(), port->name());

    UIntWord index = 12;
    TS_ASSERT_THROWS(terminal_port.setIndex(index), WrongSubclass);

    TS_ASSERT(terminal_port.isOpcodeSetting());


    // TODO: check name with hintOperation()
    TS_ASSERT_EQUALS(terminal_port.operation().name(), OP_NAME);
    
    delete hw_op;
    delete fu;
}

#endif
