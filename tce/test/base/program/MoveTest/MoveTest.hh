/** 
 * @file MoveTest.hh
 * 
 * A test suite for Move.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
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
#include "NullGuard.hh"

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

    TerminalRegister* src_reg = new TerminalRegister(
        *port1, index1);
    TerminalRegister* dst_reg = new TerminalRegister(
        *port2, index2);
    TerminalRegister* src_reg2 = new TerminalRegister(
        *port1, index1);
    TerminalRegister* dst_reg2 = new TerminalRegister(
        *port2, index2);

    MoveGuard* guard = new MoveGuard(NullGuard::instance());

    Bus moveBus("Bus1", 32, 32, Machine::ZERO);
    
    Move guardedMove(src_reg, dst_reg, moveBus, guard);
    Move unGuardedMove(src_reg2, dst_reg2, moveBus);

    TS_ASSERT_EQUALS(guardedMove.isUnconditional(), false);
    TS_ASSERT_EQUALS(unGuardedMove.isUnconditional(), true);

    TS_ASSERT_EQUALS(&guardedMove.source(), src_reg);
    TS_ASSERT_EQUALS(&unGuardedMove.source(), src_reg2);
    TS_ASSERT_EQUALS(&guardedMove.destination(), dst_reg);
    TS_ASSERT_EQUALS(&unGuardedMove.destination(), dst_reg2);

    TS_ASSERT_EQUALS(&guardedMove.guard(), guard);
    TS_ASSERT_THROWS(unGuardedMove.guard(), InvalidData);

    TS_ASSERT_EQUALS(&guardedMove.bus(), &moveBus);
    TS_ASSERT_EQUALS(&unGuardedMove.bus(), &moveBus);

    delete reg_file;
}

#endif
