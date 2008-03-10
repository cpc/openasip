/**
 * @file MachineInfoTest.hh
 *
 * A test suite for MachineInfo functions.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#ifndef MACHINE_INFO_TEST_HH
#define MACHINE_INFO_TEST_HH

#include <TestSuite.h>
#include "MachineInfo.hh"
#include "Machine.hh"

class MachineInfoTest : public CxxTest::TestSuite {
public:
    MachineInfoTest();

    void setUp();
    void tearDown();

    void testGetOpset();
private:
};

MachineInfoTest::MachineInfoTest() {
}

void 
MachineInfoTest::setUp() {
}

void 
MachineInfoTest::tearDown() {
}

/**
 * Tests that the requesting opset of machine works.
 */
void
MachineInfoTest::testGetOpset() {

    TTAMachine::Machine* targetMachine = NULL;
    
    CATCH_ANY(
        targetMachine = 
        TTAMachine::Machine::loadFromADF(
            "data/10_bus_reduced_connectivity.adf"));
    
    OperationDAGSelector::OperationSet opSet = MachineInfo::getOpset(*targetMachine);

    TS_ASSERT_DIFFERS(opSet.find("ABS"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("ABSF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("ADD"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("ADDF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("AND"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("CFI"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("CIF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("DIV"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("DIVU"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("DIVF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("EQ"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("EQF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("GT"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("GTF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("GTU"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("IOR"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("LDD"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("LDH"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("LDQ"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("LDQU"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("LDW"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MAX"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MAXU"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MIN"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MINU"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MOD"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MODU"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MUL"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("MULF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("NEG"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("NEGF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("OUTPUTDATA"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("PRINT_INT"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("ROTL"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("ROTR"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("SHL"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("SHR"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("SHRU"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("STD"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("STDOUT"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("STH"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("STQ"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("STW"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("SUB"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("SUBF"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("SXHW"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("SXQW"), opSet.end());
    TS_ASSERT_DIFFERS(opSet.find("XOR"), opSet.end());
    TS_ASSERT_EQUALS(opSet.size(), static_cast<unsigned int>(48));

    
    delete targetMachine;
}

#endif
