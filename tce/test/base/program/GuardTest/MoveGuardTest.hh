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
 * @file MoveGuardTest.hh
 * 
 * A test suite for MoveGuard.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef GUARD_TEST_HH
#define GUARD_TEST_HH

#include <TestSuite.h>
#include "MoveGuard.hh"
#include "Guard.hh"
#include "RegisterFile.hh"
#include "RFPort.hh"
#include "Bus.hh"

using namespace TTAMachine;
using namespace TTAProgram;

class MoveGuardTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testMoveGuard();

private:
};


/**
 * Called before each test.
 */
void
MoveGuardTest::setUp() {
}


/**
 * Called after each test.
 */
void
MoveGuardTest::tearDown() {
}

/**
 * Tests that MoveGuard works properly.
 */
void
MoveGuardTest::testMoveGuard() {
    Machine dummy_mach;

    RegisterFile* reg_file = new RegisterFile(
        "RF", 32, 32, 1, 1, 0, RegisterFile::NORMAL);
    UIntWord index1 = 10;
    UIntWord index2 = 14;

    Bus* bus1 = new Bus("bus1", 32, 32, Machine::SIGN);
    Bus* bus2 = new Bus("bus2", 32, 32, Machine::SIGN);

    dummy_mach.addRegisterFile(*reg_file);
    dummy_mach.addBus(*bus1);
    dummy_mach.addBus(*bus2);

    RegisterGuard guard1(false, *reg_file, index1, *bus1);
    RegisterGuard guard2(true, *reg_file, index2, *bus2);

    MoveGuard guard(guard1);
    MoveGuard invGuard(guard2);

    TS_ASSERT_EQUALS(guard.isUnconditional(), false);
    TS_ASSERT_EQUALS(invGuard.isUnconditional(), false);

    TS_ASSERT_EQUALS(guard.isInverted(), false);
    TS_ASSERT_EQUALS(invGuard.isInverted(), true);

    TS_ASSERT_EQUALS(&guard.guard(), &guard1);
    TS_ASSERT_EQUALS(&invGuard.guard(), &guard2);
}

#endif
