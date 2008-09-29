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
