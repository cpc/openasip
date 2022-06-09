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
 * @file ScopeTest.hh
 * 
 * A test suite for Scope.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 */

#ifndef SCOPE_TEST_HH
#define SCOPE_TEST_HH

#include <TestSuite.h>
#include "Program.hh"
#include "Procedure.hh"
#include "Scope.hh"
#include "GlobalScope.hh"
#include "CodeLabel.hh"
#include "DataLabel.hh"
#include "Instruction.hh"
#include "NullAddressSpace.hh"
#include "InstructionReference.hh"
#include "InstructionReferenceManager.hh"
#include "Machine.hh"

using namespace TTAMachine;
using namespace TTAProgram;

class ScopeTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testBasicFunctionality();
    void testBookkeeping();
};


/**
 * Called before each test.
 */
void
ScopeTest::setUp() {
}


/**
 * Called after each test.
 */
void
ScopeTest::tearDown() {
}


/**
 * Tests the basic functionality of scopes.
 */
void
ScopeTest::testBasicFunctionality() {
    Machine dummy_mach;
    AddressSpace as("AS", 32, 0, 100000, dummy_mach);
    Program program(as);

    GlobalScope& globalScope = program.globalScope();

    TS_ASSERT_EQUALS(globalScope.isGlobal(), true);
    TS_ASSERT_EQUALS(globalScope.isUnit(), false);
    TS_ASSERT_EQUALS(globalScope.isProcedure(), false);
    TS_ASSERT_EQUALS(globalScope.isLocal(), false);

    Procedure* proc = new Procedure("proc1", as);
    program.addProcedure(proc);

    Instruction* ins = new Instruction();
    program.addInstruction(ins);
    InstructionReference insRef =
        program.instructionReferenceManager().createReference(*ins);

    CodeLabel* codeLabel = new CodeLabel(insRef, "code_label1");
    DataLabel* dataLabel = new DataLabel(
        "data_label1", Address(100, as), globalScope);

    TS_ASSERT_THROWS_NOTHING(globalScope.addCodeLabel(codeLabel));
    TS_ASSERT_THROWS_NOTHING(globalScope.addDataLabel(dataLabel));

    TS_ASSERT_THROWS(globalScope.codeLabel("not_found"), KeyNotFound);
    TS_ASSERT_THROWS(globalScope.dataLabel("not_found"), KeyNotFound);
    TS_ASSERT_THROWS_NOTHING(globalScope.codeLabel("code_label1"));
    TS_ASSERT_THROWS_NOTHING(globalScope.dataLabel("data_label1"));
    TS_ASSERT_EQUALS(&globalScope.codeLabel("code_label1"), codeLabel);
    TS_ASSERT_EQUALS(&globalScope.dataLabel("data_label1"), dataLabel);

    TS_ASSERT_EQUALS(globalScope.containsCodeLabel("code_label1"), true);
    TS_ASSERT_EQUALS(globalScope.containsDataLabel("data_label1"), true);
    TS_ASSERT_EQUALS(globalScope.containsCodeLabel("foo"), false);
    TS_ASSERT_EQUALS(globalScope.containsDataLabel("bar"), false);

    TS_ASSERT_THROWS(globalScope.addCodeLabel(codeLabel), KeyAlreadyExists);
    TS_ASSERT_THROWS(globalScope.addDataLabel(dataLabel), KeyAlreadyExists);

    Instruction* ins2 = new Instruction();
    program.addInstruction(ins2);
    InstructionReference insRef2 =
        program.instructionReferenceManager().createReference(*ins2);

    CodeLabel* badCodeLabel = new CodeLabel(insRef2, "code_label1");

    TS_ASSERT_THROWS(
        globalScope.addCodeLabel(badCodeLabel), KeyAlreadyExists);
    delete badCodeLabel;

    DataLabel* badDataLabel = new DataLabel(
        "data_label1", Address(200, as), globalScope);
    TS_ASSERT_THROWS(
        globalScope.addDataLabel(badDataLabel), KeyAlreadyExists);
    delete badDataLabel;
}

/**
 * Tests the global label bookkeeping.
 */
void
ScopeTest::testBookkeeping() {
    Machine dummy_mach;
    AddressSpace as("AS", 32, 0, 100000, dummy_mach);
    Program program(as);

    GlobalScope& globalScope = program.globalScope();

    Procedure* proc = new Procedure("proc1", as);
    program.addProcedure(proc);

    Instruction* ins = new Instruction;
    program.addInstruction(ins);
    Instruction* ins2 = new Instruction;
    program.addInstruction(ins2);
    InstructionReference insRef =
        program.instructionReferenceManager().createReference(*ins);
    InstructionReference insRef2 =
        program.instructionReferenceManager().createReference(*ins2);

    CodeLabel* codeLabel1 = new CodeLabel(insRef, "code_label1");
    CodeLabel* codeLabel2 = new CodeLabel(insRef, "code_label2");
    CodeLabel* codeLabel3 = new CodeLabel(insRef2, "code_label3");
    CodeLabel* codeLabel4 = new CodeLabel(insRef2, "code_label4");
    CodeLabel* codeLabel5 = new CodeLabel(insRef2, "code_label5");

    globalScope.addCodeLabel(codeLabel1);
    globalScope.addCodeLabel(codeLabel2);
    globalScope.addCodeLabel(codeLabel3);
    globalScope.addCodeLabel(codeLabel4);
    globalScope.addCodeLabel(codeLabel5);

    TS_ASSERT_EQUALS(
        &globalScope.globalCodeLabel(ins->address(), 0), codeLabel1);
    TS_ASSERT_EQUALS(
        &globalScope.globalCodeLabel(ins->address(), 1), codeLabel2);
    TS_ASSERT_EQUALS(
        &globalScope.globalCodeLabel(ins2->address(), 0), codeLabel3);
    TS_ASSERT_EQUALS(
        &globalScope.globalCodeLabel(ins2->address(), 1), codeLabel4);
    TS_ASSERT_EQUALS(
        &globalScope.globalCodeLabel(ins2->address(), 2), codeLabel5);

    TS_ASSERT_EQUALS(globalScope.codeLabel("code_label1").address().location(),
        ins->address().location());
    TS_ASSERT_EQUALS(
        globalScope.codeLabel("code_label2").address().location(),
        ins->address().location());
    TS_ASSERT_EQUALS(
        globalScope.codeLabel("code_label3").address().location(),
        ins2->address().location());
    TS_ASSERT_EQUALS(
        globalScope.codeLabel("code_label4").address().location(),
        ins2->address().location());
    
    Address label1Addr(10, NullAddressSpace::instance());
    Address label2Addr(1999, NullAddressSpace::instance());

    DataLabel* dataLabel1 = new DataLabel(
        "data_label1", label1Addr, globalScope);
    DataLabel* dataLabel2 = new DataLabel(
        "data_label2", label2Addr, globalScope);
    DataLabel* dataLabel3 = new DataLabel(
        "data_label3", label2Addr, globalScope);

    TS_ASSERT_EQUALS(globalScope.globalDataLabelCount(label1Addr), 0);
    TS_ASSERT_EQUALS(globalScope.globalDataLabelCount(label2Addr), 0);

    globalScope.addDataLabel(dataLabel1);
    globalScope.addDataLabel(dataLabel2);
    globalScope.addDataLabel(dataLabel3);

    TS_ASSERT_EQUALS(globalScope.globalDataLabelCount(label1Addr), 1);
    TS_ASSERT_EQUALS(globalScope.globalDataLabelCount(label2Addr), 2);

    TS_ASSERT_EQUALS(
        &globalScope.globalDataLabel(label1Addr, 0), dataLabel1);
    TS_ASSERT_EQUALS(
        &globalScope.globalDataLabel(label2Addr, 0), dataLabel2);
    TS_ASSERT_EQUALS(
        &globalScope.globalDataLabel(label2Addr, 1), dataLabel3);

    TS_ASSERT_EQUALS(
        globalScope.dataLabel("data_label1").address().location(),
        label1Addr.location());
    TS_ASSERT_EQUALS(
        globalScope.dataLabel("data_label2").address().location(),
        label2Addr.location());
    TS_ASSERT_EQUALS(
        globalScope.dataLabel("data_label3").address().location(),
        label2Addr.location());
}

#endif
