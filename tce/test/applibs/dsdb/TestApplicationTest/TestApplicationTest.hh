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
 * @file TestApplicationTest.hh
 *
 * Test for the TestApplication class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
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
