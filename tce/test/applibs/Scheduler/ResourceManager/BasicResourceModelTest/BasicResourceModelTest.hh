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
