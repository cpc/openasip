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
 * @file ProgramTest.hh
 *
 * A test suite for Program.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef PROGRAM_TEST_HH
#define PROGRAM_TEST_HH

#include <iostream>
using std::cerr;
using std::endl;

#include <TestSuite.h>
#include "Program.hh"
#include "Procedure.hh"
#include "NullProcedure.hh"
#include "Instruction.hh"
#include "NullInstruction.hh"
#include "AddressSpace.hh"
#include "Machine.hh"


using namespace TTAMachine;
using namespace TTAProgram;

class ProgramTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testBasicFunctions();
    void testProcedureHandling();
    void testInstructionHandling();
};


/**
 * Called before each test.
 */
void
ProgramTest::setUp() {
}


/**
 * Called after each test.
 */
void
ProgramTest::tearDown() {
}

/**
 * Tests the basic functionality of Program, such as construction, setters
 * and getters.
 */
void
ProgramTest::testBasicFunctions() {

    Machine dummy_mach;
    AddressSpace as("AS", 32, 0, 100000, dummy_mach);
    Program prog1(as);

    TS_ASSERT_EQUALS(static_cast<int>(prog1.startAddress().location()), 0);
    TS_ASSERT_EQUALS(static_cast<int>(prog1.entryAddress().location()), 0);

    TS_ASSERT_EQUALS(&prog1.startAddress().space(), &as);
    TS_ASSERT_EQUALS(&prog1.entryAddress().space(), &as);

    Address addr1(100, as);
    Program prog2(as, addr1);

    TS_ASSERT_EQUALS(prog2.startAddress().location(), addr1.location());
    TS_ASSERT_EQUALS(static_cast<int>(prog2.entryAddress().location()), 0);

    TS_ASSERT_EQUALS(&prog2.startAddress().space(), &as);
    TS_ASSERT_EQUALS(&prog2.entryAddress().space(), &as);

    Address addr2(200, as);
    Program prog3(as, addr1, addr2);

    TS_ASSERT_EQUALS(prog3.startAddress().location(), addr1.location());
    TS_ASSERT_EQUALS(prog3.entryAddress().location(), addr2.location());

    TS_ASSERT_EQUALS(&prog3.startAddress().space(), &as);
    TS_ASSERT_EQUALS(&prog3.entryAddress().space(), &as);

    Address addr3(300, as);
    prog1.setEntryAddress(addr3);

    TS_ASSERT_EQUALS(prog1.entryAddress().location(), addr3.location());
}

/**
 * Tests procedure handling.
 */
void
ProgramTest::testProcedureHandling() {

    Machine dummy_mach;
    AddressSpace as1("AS1", 32, 0, 99, dummy_mach);
    Program prog1(as1);

    UIntWord addr1 = 100;
    Procedure* proc1 = new Procedure("proc1", as1, addr1);

    TS_ASSERT_THROWS_NOTHING(prog1.addProcedure(proc1));
    TS_ASSERT_EQUALS(
        proc1->startAddress().location(), prog1.startAddress().location());

    Procedure* proc2 = new Procedure("proc2", as1, addr1);
    Procedure* proc3 = new Procedure("proc3", as1, addr1);

    Instruction* ins1 = new Instruction();
    Instruction* ins2 = new Instruction();
    Instruction* ins3 = new Instruction();

    proc1->add(ins1);
    proc1->add(ins2);
    proc2->add(ins3);

    TS_ASSERT_THROWS_NOTHING(prog1.addProcedure(proc2));
    TS_ASSERT_THROWS_NOTHING(prog1.addProcedure(proc3));

    TS_ASSERT_EQUALS(
        proc2->startAddress().location(), proc1->endAddress().location());
    TS_ASSERT_EQUALS(
        proc3->startAddress().location(), proc2->endAddress().location());

    TS_ASSERT_EQUALS(static_cast<int>(proc3->endAddress().location()), 3);

    AddressSpace badAS("BADAS", 32, 100, 400000, dummy_mach);
    Program badProg(badAS);
    UIntWord addr2 = 100;
    Procedure* badProc = new Procedure("badProc", badAS, addr2);

    TS_ASSERT_THROWS_NOTHING(badProg.addProcedure(badProc));
    TS_ASSERT_THROWS(prog1.addProcedure(badProc), IllegalRegistration);

    TS_ASSERT_EQUALS(prog1.procedureCount(), 3);
    TS_ASSERT_EQUALS(&prog1.procedure(1), proc2);
    TS_ASSERT_THROWS(prog1.procedure(3), OutOfRange);

    TS_ASSERT_EQUALS(&prog1.firstProcedure(), proc1);
    TS_ASSERT_EQUALS(&prog1.lastProcedure(), proc3);
    TS_ASSERT_EQUALS(&prog1.nextProcedure(*proc1), proc2);
    TS_ASSERT_EQUALS(&prog1.nextProcedure(*proc2), proc3);
    TS_ASSERT_EQUALS(
        &prog1.nextProcedure(*proc3), &NullProcedure::instance());
    TS_ASSERT_EQUALS(prog1.globalScope().isGlobal(), true);

    // test that labels for procedures were automatically added.
#if 0
    TS_ASSERT_EQUALS(prog1.globalScope().containsCodeLabel("proc1"), true);
    TS_ASSERT_EQUALS(prog1.globalScope().containsCodeLabel("proc2"), true);
    TS_ASSERT_EQUALS(prog1.globalScope().containsCodeLabel("proc3"), true);
    TS_ASSERT_EQUALS(
        prog1.globalScope().containsCodeLabel("badProc"), false);

    TS_ASSERT_EQUALS(
        prog1.globalScope().addressOfCodeLabel("proc2").location(),
        proc2->startAddress().location());
#endif

    TS_ASSERT_EQUALS(prog1.procedureCount(), 3);
    TS_ASSERT_THROWS_NOTHING(prog1.removeProcedure(*proc2));
    TS_ASSERT_EQUALS(prog1.procedureCount(), 2);
    TS_ASSERT_EQUALS(
        proc3->startAddress().location(), proc1->endAddress().location());
    TS_ASSERT_THROWS(prog1.removeProcedure(*proc2), IllegalRegistration);
}

/**
 * Tests instruction handling.
 */
void
ProgramTest::testInstructionHandling() {

    Machine dummy_mach;
    AddressSpace as1("AS1", 32, 0, 99, dummy_mach);
    Program prog1(as1);

    Instruction* ins1 = new Instruction;

    TS_ASSERT_THROWS(prog1.addInstruction(ins1), IllegalRegistration);

    UIntWord addr1 = 0;
    Procedure* proc1 = new Procedure("proc1", as1, addr1);

    prog1.addProcedure(proc1);

    TS_ASSERT_THROWS_NOTHING(prog1.addInstruction(ins1));

    Procedure* proc2 = new Procedure("proc2", as1, addr1);
    prog1.addProcedure(proc2);

    TS_ASSERT_EQUALS(
        proc2->startAddress().location(), proc1->endAddress().location());

    Instruction* ins2 = new Instruction;
    Instruction* ins3 = new Instruction;
    Instruction* ins4 = new Instruction;

    TS_ASSERT_THROWS_NOTHING(prog1.addInstruction(ins2));
    TS_ASSERT_THROWS_NOTHING(prog1.addInstruction(ins3));
    TS_ASSERT_THROWS_NOTHING(prog1.addInstruction(ins4));

    Procedure* proc3 = new Procedure("proc3", as1, addr1);
    prog1.addProcedure(proc3);

    TS_ASSERT_EQUALS(
        proc3->startAddress().location(), proc2->endAddress().location());

    Instruction* ins5 = new Instruction();

    TS_ASSERT_THROWS_NOTHING(proc1->add(ins5));
    TS_ASSERT_EQUALS(static_cast<int>(proc2->startAddress().location()), 2);

    TS_ASSERT_EQUALS(
        proc3->startAddress().location(), proc2->endAddress().location());

    TS_ASSERT_THROWS_NOTHING(proc2->remove(*ins4));
    TS_ASSERT_THROWS(proc2->remove(*ins4), IllegalRegistration);
    TS_ASSERT_EQUALS(
        proc3->startAddress().location(), proc2->endAddress().location());
    TS_ASSERT_THROWS_NOTHING(proc2->add(ins4));
    TS_ASSERT_THROWS(proc1->remove(*ins4), IllegalRegistration);
    TS_ASSERT_EQUALS(
        proc3->startAddress().location(), proc2->endAddress().location());

    Procedure* proc4 = new Procedure("proc4", as1, addr1);
    Instruction* badIns = new Instruction();
    proc4->add(badIns);

    TS_ASSERT_THROWS(prog1.addInstruction(badIns), IllegalRegistration);

    TS_ASSERT_EQUALS(&prog1.nextInstruction(*ins1), ins5);
    TS_ASSERT_EQUALS(&prog1.nextInstruction(*ins5), ins2);
    TS_ASSERT_EQUALS(&prog1.nextInstruction(*ins2), ins3);
    TS_ASSERT_EQUALS(&prog1.nextInstruction(*ins3), ins4);

    TS_ASSERT_EQUALS(
        &prog1.nextInstruction(*ins4), &NullInstruction::instance());

    TS_ASSERT_THROWS(prog1.nextInstruction(*badIns), IllegalRegistration);

    delete proc4;

    TS_ASSERT_THROWS(
        prog1.instructionAt(proc2->endAddress().location() + 1),
        KeyNotFound);

    Instruction* ins6 = new Instruction;
    Instruction* ins7 = new Instruction;

    TS_ASSERT_EQUALS(proc1->instructionCount(), 2);
    TS_ASSERT_EQUALS(
        proc2->startAddress().location(), proc1->endAddress().location());
    TS_ASSERT_THROWS_NOTHING(proc1->insertAfter(*ins1, ins6));
    TS_ASSERT_EQUALS(proc1->instructionCount(), 3);
    TS_ASSERT_EQUALS(
        proc2->startAddress().location(), proc1->endAddress().location());

    TS_ASSERT_EQUALS(proc2->instructionCount(), 3);
    TS_ASSERT_EQUALS(
        proc3->startAddress().location(), proc2->endAddress().location());
    TS_ASSERT_THROWS_NOTHING(proc2->insertAfter(*ins3, ins7));
    TS_ASSERT_EQUALS(proc2->instructionCount(), 4);
    TS_ASSERT_EQUALS(
        proc2->startAddress().location(), proc1->endAddress().location());
    TS_ASSERT_EQUALS(&prog1.nextInstruction(*ins3), ins7);
}

#endif
