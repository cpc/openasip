/** 
 * @file ImmediateTest.hh
 * 
 * A test suite for Immediate.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef IMMEDIATE_TEST_HH
#define IMMEDIATE_TEST_HH

#include <TestSuite.h>
#include <iostream>
#include "Immediate.hh"
#include "TerminalRegister.hh"
#include "TerminalImmediate.hh"
#include "Machine.hh"

using namespace TTAMachine;
using namespace TTAProgram;

class ImmediateTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testImmediate();

private:
};


/**
 * Called before each test.
 */
void
ImmediateTest::setUp() {
}


/**
 * Called after each test.
 */
void
ImmediateTest::tearDown() {
}

/**
 * Tests that Immediate works properly.
 */
void
ImmediateTest::testImmediate() {

    RegisterFile* reg_file = new RegisterFile(
        "RF", 32, 32, 1, 1, 0, RegisterFile::NORMAL);
    RFPort* port1 = new RFPort("P1", *reg_file);
    UIntWord index1 = 10;
    
    TerminalRegister*  reg_dst = new TerminalRegister(
        *port1, index1);

    Machine dummy_mach;
    InstructionTemplate ins_template("Morjes", dummy_mach);

    SimValue imm_value = SimValue(1232, 32);
    
    TerminalImmediate* test_value = new TerminalImmediate(imm_value);

    TerminalRegister* test_dst = new TerminalRegister(
        *port1, index1);

    TS_ASSERT_THROWS_NOTHING(
        Immediate imm(test_value, test_dst));

    Bus bus("bus1", 32, 32, Machine::ZERO);
    dummy_mach.addBus(bus);
    ImmediateUnit* imm_unit1 = new ImmediateUnit(
        "imm1", 32, 32, 1, 0, Machine::ZERO);
    RFPort* port2 = new RFPort("P2", *imm_unit1);
    dummy_mach.addImmediateUnit(*imm_unit1);
    
    ins_template.addSlot(bus.name(), 32, *imm_unit1);
    
    TerminalRegister* good_imm_dst = new TerminalRegister(
        *port2, index1);

    test_value = new TerminalImmediate(imm_value);
    TS_ASSERT_THROWS_NOTHING(
        Immediate imm(test_value, good_imm_dst));

    ImmediateUnit* imm_unit2 = new ImmediateUnit(
        "imm2", 32, 32, 1, 0, Machine::ZERO);

    dummy_mach.addImmediateUnit(*imm_unit2);
    
    test_value = new TerminalImmediate(imm_value);
    Immediate imm(test_value, reg_dst);
    TS_ASSERT_EQUALS(reg_dst, &imm.destination());
    TS_ASSERT_EQUALS(imm_value, imm.value().value());

    delete reg_file;
}

#endif
