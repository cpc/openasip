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
#include "InstructionReference.hh"
#include "InstructionReferenceManager.hh"
#include "Machine.hh"
#include "GlobalScope.hh"

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
    InstructionReference badInsRef =
        program.instructionReferenceManager().createReference(*badIns);

    TS_ASSERT_THROWS(
        CodeLabel testCode(badInsRef, CODE_1_NAME), IllegalRegistration);

    delete badIns;

    Procedure* proc = new Procedure("proc1", as);
    program.addProcedure(proc);
    Instruction* ins = new Instruction();
    program.addInstruction(ins);
    InstructionReference insRef =
        program.instructionReferenceManager().createReference(*ins);

    TS_ASSERT_THROWS_NOTHING(CodeLabel testCode(insRef, CODE_1_NAME));

    CodeLabel code1(insRef, CODE_1_NAME);

    TS_ASSERT_EQUALS(code1.name(), CODE_1_NAME);
    TS_ASSERT_EQUALS(code1.address().location(), ins->address().location());
    TS_ASSERT_EQUALS(&code1.scope(), &globalScope);
    TS_ASSERT_EQUALS(&code1.procedure(), &ins->parent());
}

#endif
