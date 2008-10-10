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
 * @file LabelTest.hh
 *
 * A test suite for Label.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 */

#ifndef LABEL_TEST_HH
#define LABEL_TEST_HH

#include <TestSuite.h>
#include <string>
#include "Program.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "DataLabel.hh"
#include "CodeLabel.hh"
#include "AddressSpace.hh"

using std::string;
using namespace TTAMachine;
using namespace TTAProgram;

class LabelTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testLabels();
};


/**
 * Called before each test.
 */
void
LabelTest::setUp() {
}


/**
 * Called after each test.
 */
void
LabelTest::tearDown() {
}

/**
 * Tests that labels work correctly.
 */
void
LabelTest::testLabels() {

    Machine dummy_mach;
    AddressSpace as("AS", 32, 0, 100000, dummy_mach);
    Program program(as);

    GlobalScope& globalScope = program.globalScope();

    const string DATA_1_NAME = "data1";
    const Address DATA_1_ADDR(100, as);

    DataLabel data1(DATA_1_NAME, DATA_1_ADDR, globalScope);

    TS_ASSERT_EQUALS(data1.name(), DATA_1_NAME);
    TS_ASSERT_EQUALS(data1.address().location(), DATA_1_ADDR.location());
    TS_ASSERT_EQUALS(&data1.scope(), &globalScope);

    const string CODE_1_NAME = "code1";

    Instruction* badIns = new Instruction();
    InstructionReference& badInsRef =
        program.instructionReferenceManager().createReference(*badIns);

    TS_ASSERT_THROWS(
        CodeLabel testCode(badInsRef, CODE_1_NAME), IllegalRegistration);

    delete badIns;

    Procedure* proc = new Procedure("proc1", as);
    program.addProcedure(proc);
    Instruction* ins = new Instruction();
    program.addInstruction(ins);
    InstructionReference& insRef =
        program.instructionReferenceManager().createReference(*ins);

    TS_ASSERT_THROWS_NOTHING(CodeLabel testCode(insRef, CODE_1_NAME));

    CodeLabel code1(insRef, CODE_1_NAME);

    TS_ASSERT_EQUALS(code1.name(), CODE_1_NAME);
    TS_ASSERT_EQUALS(code1.address().location(), ins->address().location());
    TS_ASSERT_EQUALS(&code1.scope(), &globalScope);
    TS_ASSERT_EQUALS(&code1.procedure(), &ins->parent());
}

#endif
