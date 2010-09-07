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
 * @file MoveTest.hh
 * 
 * A test suite for Move.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef MOVE_TEST_HH
#define MOVE_TEST_HH

#include <TestSuite.h>
#include "Move.hh"
#include "TerminalRegister.hh"
#include "MoveGuard.hh"
#include "RegisterFile.hh"
#include "Bus.hh"

using namespace TTAMachine;
using namespace TTAProgram;

class MoveTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testMove();

private:
};


/**
 * Called before each test.
 */
void
MoveTest::setUp() {
}


/**
 * Called after each test.
 */
void
MoveTest::tearDown() {
}

/**
 * Tests that Move works properly.
 */
void
MoveTest::testMove() {

    RegisterFile* reg_file = new RegisterFile(
        "RF", 32, 32, 1, 1, 0, RegisterFile::NORMAL);
    RFPort* port1 = new RFPort("P2", *reg_file);
    RFPort* port2 = new RFPort("P3", *reg_file);
    UIntWord index1 = 11;
    UIntWord index2 = 12;

    TerminalRegister* src_reg2 = new TerminalRegister(
        *port1, index1);
    TerminalRegister* dst_reg2 = new TerminalRegister(
        *port2, index2);

    Bus moveBus("Bus1", 32, 32, Machine::ZERO);
    
    Move unGuardedMove(src_reg2, dst_reg2, moveBus);

    TS_ASSERT_EQUALS(unGuardedMove.isUnconditional(), true);

    TS_ASSERT_EQUALS(&unGuardedMove.source(), src_reg2);
    TS_ASSERT_EQUALS(&unGuardedMove.destination(), dst_reg2);

    TS_ASSERT_THROWS(unGuardedMove.guard(), InvalidData);

    TS_ASSERT_EQUALS(&unGuardedMove.bus(), &moveBus);

    delete reg_file;
}

#endif
