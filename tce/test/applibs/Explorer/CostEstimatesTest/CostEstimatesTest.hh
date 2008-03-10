/** 
 * @file CostEstimatesTest.hh 
 *
 * A test suite for CostEstimates class.
 * 
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_COST_ESTIMATES_TEST_HH
#define TTA_COST_ESTIMATES_TEST_HH

#include <string>
#include <TestSuite.h>
#include <map>

#include "CostEstimates.hh"
#include "Program.hh"
#include "Machine.hh"
#include "AddressSpace.hh"

/**
 * Class that tests CostEstimates class.
 */
class CostEstimatesTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testEstimates();

private:

};


/**
 * Called before each test.
 */
void
CostEstimatesTest::setUp() {
}


/**
 * Called after each test.
 */
void
CostEstimatesTest::tearDown() {
}


/**
 * Test CostEstimates.
 */
void
CostEstimatesTest::testEstimates() {

    CostEstimates data;
    double area = 12.0;
    double delay = 23.0;
    data.setArea(area);
    data.setLongestPathDelay(delay);

    TS_ASSERT_EQUALS(data.area(), area);
    TS_ASSERT_EQUALS(data.longestPathDelay(), delay);

    // reset the area and longest path delay
    area = 42.1;
    delay = 22.3;
    data.setArea(area);
    data.setLongestPathDelay(delay);

    TS_ASSERT_EQUALS(data.area(), area);
    TS_ASSERT_EQUALS(data.longestPathDelay(), delay);

    TS_ASSERT_EQUALS(data.energies(), 0);
    TTAMachine::Machine machine;
    TTAMachine::AddressSpace space("space", 32, 0, 1000, machine);
    TTAProgram::Program program(space);
    double energy = 0.1;
    data.setEnergy(program, energy);

    TS_ASSERT_EQUALS(data.energy(program), energy);
    TS_ASSERT_EQUALS(data.energies(), 1);
    
    TTAMachine::AddressSpace space2("space2", 32, 1001, 2000, machine);
    TTAProgram::Program program2(space2);
    double energy2 = 22.2;
    data.setEnergy(program2, energy2);

    TS_ASSERT_EQUALS(data.energy(program), energy);
    TS_ASSERT_EQUALS(data.energy(program2), energy2);
    TS_ASSERT_EQUALS(data.energies(), 2);

    // test with program that has no energy value set
    TTAProgram::Program program3(space2);
    TS_ASSERT_THROWS(data.energy(program3), KeyNotFound);

    data.setEnergy(program2, energy);
    TS_ASSERT_EQUALS(data.energy(program2), energy);

    // test assignment.
    CostEstimates test;
    test = data;
    TS_ASSERT_EQUALS(test.area(), area);
    TS_ASSERT_EQUALS(test.longestPathDelay(), delay);
    TS_ASSERT_EQUALS(test.energy(program), energy);
    TS_ASSERT_EQUALS(test.energy(program2), energy);
    TS_ASSERT_EQUALS(test.energies(), 2);
}

#endif
