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
