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
 * @file TestApplicationTest.hh
 *
 * Test for the TestApplication class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_TEST_APPLICATION_TEST_HH
#define TTA_TEST_APPLICATION_TEST_HH

#include <TestSuite.h>
#include <string>
#include <vector>
#include <sstream>

#include "TestApplication.hh"
#include "BaseLineReader.hh"

const std::string TEST_DIRECTORY_PATH = "data/Test";

class TestApplicationTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testDescription();
    void testVerify();
    void testMaxRuntime();
    void testSimulateTTASim();

private:
};

/**
 * Called before each test.
 */
void
TestApplicationTest::setUp() {
}


/**
 * Called after each test.
 */
void
TestApplicationTest::tearDown() {
}

/**
 * Tests getting the description.
 */
void
TestApplicationTest::testDescription() {

    TestApplication test(TEST_DIRECTORY_PATH);
    std::vector<std::string> description = test.description();

    std::vector<std::string> expectedResult;
    expectedResult.push_back("This is a test for TestApplication class.\n");
    expectedResult.push_back("Testing another line");

    TS_ASSERT_EQUALS(description, expectedResult);
}

/**
 * Tests verifying the simulation output.
 */
void
TestApplicationTest::testVerify() {

    TestApplication test(TEST_DIRECTORY_PATH);
    TS_ASSERT(test.hasVerifySimulation());
    TS_ASSERT(test.verifySimulation());
}

/**
 * Test that max runtime is read correcly.
 */
void
TestApplicationTest::testMaxRuntime() {

    TestApplication test(TEST_DIRECTORY_PATH);
    TS_ASSERT_EQUALS(test.maxRuntime(), 234.567);
}

void
TestApplicationTest::testSimulateTTASim() {

    TestApplication test(TEST_DIRECTORY_PATH);
    std::istream* input = test.simulateTTASim();
    BaseLineReader reader(*input);
    reader.initialize();
    std::string command = "";
    command = reader.readLine();
    TS_ASSERT_EQUALS(command, "foo");
    command = reader.readLine();
    TS_ASSERT_EQUALS(command, "bar");
    command = reader.readLine();
    TS_ASSERT_EQUALS(command, "x /u w __profiling");
}

#endif
