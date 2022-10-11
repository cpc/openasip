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
 * @file ProcedureTest.hh
 *
 * A test suite for Procedure.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef PROCEDURE_TEST_HH
#define PROCEDURE_TEST_HH

#include <iostream>
using std::cout;
using std::endl;

#include <TestSuite.h>
#include "Procedure.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "NullInstruction.hh"
#include "AddressSpace.hh"
#include "Machine.hh"
#include "Conversion.hh"

using namespace TTAMachine;
using namespace TTAProgram;

class ProcedureTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testBasicFunctions();
    void testInstructionHandling();
};


/**
 * Called before each test.
 */
void
ProcedureTest::setUp() {
}


/**
 * Called after each test.
 */
void
ProcedureTest::tearDown() {
}

/**
 * Tests that procedure is constructed correctly and other basic functions.
 */
void
ProcedureTest::testBasicFunctions() {

    Machine dummy_mach;
    AddressSpace as("AS", 32, 0, 100000, dummy_mach);
    UIntWord addr1 = 0;
    UIntWord addr2 = 0;
    Procedure* proc1 = new Procedure("proc1", as, addr1);
    Procedure* proc2 = new Procedure("proc2", as, addr2);

    TS_ASSERT_THROWS(proc1->parent(), IllegalRegistration);
    TS_ASSERT_THROWS(proc2->parent(), IllegalRegistration);
    TS_ASSERT_EQUALS(proc1->isInProgram(), false);
    TS_ASSERT_EQUALS(proc2->isInProgram(), false);

    Program prog(as);

    prog.addProcedure(proc1);
    prog.addProcedure(proc2);

    TS_ASSERT_EQUALS(proc1->isInProgram(), true);
    TS_ASSERT_EQUALS(proc2->isInProgram(), true);

    TS_ASSERT_EQUALS(&proc1->parent(), &prog);
    TS_ASSERT_EQUALS(&proc2->parent(), &prog);

    TS_ASSERT_EQUALS(proc1->startAddress().location(), addr1);
    TS_ASSERT_EQUALS(proc2->startAddress().location(), addr2);

    Address addr3(70000, as);
    proc2->setStartAddress(addr3);
    TS_ASSERT_EQUALS(proc2->startAddress().location(), addr3.location());
}

/**
 * Tests adding instructions and instruction lookup.
 */
void
ProcedureTest::testInstructionHandling() {

    Machine dummy_mach;
    AddressSpace as("AS", 32, 0, 100000, dummy_mach);
    UIntWord addr = 40;
    UIntWord addr2 =111110;
    Procedure* proc = new Procedure("proc", as, addr);
    Procedure* proc2 = new Procedure("proc2", as, addr2);

    Program prog(as);

    TS_ASSERT_EQUALS(proc->endAddress().location(), addr);
    TS_ASSERT_EQUALS(proc2->endAddress().location(), addr2);

    Instruction* ins1 = new Instruction();
    Instruction* ins2 = new Instruction();
    Instruction* ins3 = new Instruction();

    TS_ASSERT_THROWS_NOTHING(proc->add(ins1));
    TS_ASSERT_THROWS_NOTHING(proc->add(ins2));
    TS_ASSERT_THROWS_NOTHING(proc->add(ins3));

    TS_ASSERT_EQUALS(proc->endAddress().location(), addr + 3);

    int badAddress = proc->endAddress().location() + 100;

    TS_ASSERT_THROWS(proc->instructionAt(badAddress), KeyNotFound);
    TS_ASSERT_EQUALS(
        &proc->instructionAt(proc->startAddress().location() + 2),
        ins3);
    badAddress = proc->address(*ins2).location() + 5;

    TS_ASSERT_THROWS(proc->instructionAt(badAddress), KeyNotFound);
    Instruction* badIns = new Instruction();
    badIns->setParent(*proc2);

    TS_ASSERT_THROWS(proc->add(badIns), IllegalRegistration);
    TS_ASSERT_THROWS(proc->nextInstruction(*badIns), IllegalRegistration);
    CATCH_ANY(proc->hasNextInstruction(*ins1));
    TS_ASSERT_EQUALS(proc->hasNextInstruction(*ins1), true);
    TS_ASSERT_EQUALS(&proc->nextInstruction(*ins1), ins2);
    TS_ASSERT_EQUALS(proc->hasNextInstruction(*ins2), true);
    TS_ASSERT_EQUALS(&proc->nextInstruction(*ins2), ins3);
    TS_ASSERT_EQUALS(proc->hasNextInstruction(*ins3), false);
    TS_ASSERT_EQUALS(
        &proc->nextInstruction(*ins3), &NullInstruction::instance());

    TS_ASSERT_EQUALS(static_cast<int>(proc->address(*ins1).location()), 40);
    TS_ASSERT_EQUALS(static_cast<int>(proc->address(*ins2).location()), 41);
    TS_ASSERT_EQUALS(static_cast<int>(proc->address(*ins3).location()), 42);
    TS_ASSERT_THROWS(proc->address(*badIns), IllegalRegistration);

    Instruction* ins4 = new Instruction();
    Instruction* ins5 = new Instruction();
    Instruction* ins6 = new Instruction();
    Instruction* ins7 = new Instruction();

    TS_ASSERT_EQUALS(proc->instructionCount(), 3);

    TS_ASSERT_THROWS_NOTHING(proc->insertAfter(*ins3, ins4));

    TS_ASSERT_EQUALS(proc->instructionCount(), 4);

    TS_ASSERT_EQUALS(&proc->nextInstruction(*ins1), ins2);
    TS_ASSERT_EQUALS(&proc->nextInstruction(*ins2), ins3);
    TS_ASSERT_EQUALS(&proc->nextInstruction(*ins3), ins4);
    TS_ASSERT_EQUALS(
        &proc->nextInstruction(*ins4), &NullInstruction::instance());

    proc->insertAfter(*ins4, ins5);
    TS_ASSERT_EQUALS(&proc->nextInstruction(*ins4), ins5);
    TS_ASSERT_EQUALS(
        &proc->nextInstruction(*ins5), &NullInstruction::instance());

    TS_ASSERT_THROWS(
        proc->insertAfter(*ins6, ins7), IllegalRegistration);

    TS_ASSERT_THROWS(
        proc->insertAfter(*badIns, ins7),
        IllegalRegistration);

    proc->insertAfter(*ins1, ins6);

    TS_ASSERT_EQUALS(&proc->nextInstruction(*ins1), ins6);
    TS_ASSERT_EQUALS(&proc->nextInstruction(*ins6), ins2);
    proc->insertAfter(*ins1, ins7);
    TS_ASSERT_EQUALS(&proc->nextInstruction(*ins1), ins7);
    TS_ASSERT_EQUALS(&proc->nextInstruction(*ins7), ins6);

    Instruction* ins8 = new Instruction();

    TS_ASSERT_THROWS_NOTHING(proc->insertBefore(*ins7, ins8));
    TS_ASSERT_EQUALS(&proc->nextInstruction(*ins8), ins7);

    delete proc;
    delete proc2;
    delete badIns;
}

#endif
