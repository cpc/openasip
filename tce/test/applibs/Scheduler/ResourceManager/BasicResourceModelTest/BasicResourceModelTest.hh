/**
 * @file BasicResourceModelTest.hh
 *
 * A test suite for the construction adn functionality of Resource Model
 *
 * @author Vladimir Guzma 2007 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef BASIC_RESOURCE_MODEL_TEST_HH
#define BASIC_RESOURCE_MODEL_TEST_HH
#include <iostream>
#include <TestSuite.h>
#include "SimpleResourceManager.hh"
#include "UniversalMachine.hh"

/**
 * Tests basic functionality of the Resource Model using
 * SimpleResourceManager.
 */
class BasicResourceModelTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testBasicConstruction();
};


void
BasicResourceModelTest::setUp() {
}

void
BasicResourceModelTest::tearDown() {
}

/**
 * Tests basic functionality by constructing Resource Model
 * of ADF and doing self tests on resources.
 * Construction throws exception in case of internal error.
 */
void
BasicResourceModelTest::testBasicConstruction() {

    TTAMachine::Machine* targetMachine1 = NULL;
    TTAMachine::Machine* targetMachine2 = NULL;
    TTAMachine::Machine* targetMachine3 = NULL;

#define DEBUG_OUTPUT
    CATCH_ANY(
        targetMachine1 =
        TTAMachine::Machine::loadFromADF(
            "data/1_bus_full_connectivity.adf"));
    TS_ASSERT_THROWS_NOTHING(SimpleResourceManager rm1(*targetMachine1));

    CATCH_ANY(
        targetMachine2 =
        TTAMachine::Machine::loadFromADF(
            "data/10_bus_full_connectivity.adf"));
    TS_ASSERT_THROWS_NOTHING(SimpleResourceManager rm2(*targetMachine2));

    CATCH_ANY(
        targetMachine3 =
        TTAMachine::Machine::loadFromADF(
            "data/10_bus_reduced_connectivity.adf-disabled"));
    TS_ASSERT_THROWS_NOTHING(SimpleResourceManager rm3(*targetMachine3));


    delete targetMachine1;
    delete targetMachine2;
    delete targetMachine3;

}

#endif
