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
 * @file ApplicationTest.hh
 * 
 * A test suite for Application helper class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 */

#ifndef APPLICATION_TEST_HH
#define APPLICATION_TEST_HH

#include <vector>
#include <string>
using std::vector;
using std::string;

#include <TestSuite.h>

#include "Application.hh"

class ApplicationTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testPopenWrapper();

    void testMisc();

private:
};


/**
 * Called before each test.
 */
void
ApplicationTest::setUp() {
}


/**
 * Called after each test.
 */
void
ApplicationTest::tearDown() {
}


/**
 * Tests the "popen() wrapper" runShellCommandAndGetOutput().
 *
 */
void
ApplicationTest::testPopenWrapper() {
    vector<string> output;

    TS_ASSERT_EQUALS(
	Application::runShellCommandAndGetOutput(
	    "data/success.sh", output), 0);
    TS_ASSERT_EQUALS(static_cast<int>(output.size()), 4);
    TS_ASSERT_EQUALS(output[0], "Hello\n");
    TS_ASSERT_EQUALS(output[1], "My name is\n");
    TS_ASSERT_EQUALS(output[2], "My name is\n");
    TS_ASSERT_EQUALS(output[3], "Pekka shady\n");

    output.clear();
    TS_ASSERT_EQUALS(
	Application::runShellCommandAndGetOutput(
	    "data/failure.sh", output), 6);
    TS_ASSERT_EQUALS(static_cast<int>(output.size()), 2);
    TS_ASSERT_EQUALS(output[0], "Oh..\n");
    TS_ASSERT_EQUALS(output[1], "I'm such a failure!\n");

    output.clear();
    TS_ASSERT_EQUALS(
	Application::runShellCommandAndGetOutput(
	    "data/failure.sh", output, 1), 6);
    TS_ASSERT_EQUALS(static_cast<int>(output.size()), 1);
    TS_ASSERT_EQUALS(output[0], "Oh..\n");

    output.clear();
    TS_ASSERT_EQUALS(
	Application::runShellCommandAndGetOutput(
	    "data/empty_failure.sh", output), 1);
    TS_ASSERT_EQUALS(static_cast<int>(output.size()), 0);

    output.clear();
    TS_ASSERT_EQUALS(
	Application::runShellCommandAndGetOutput(
	    "data/empty_success.sh", output), 0);
    TS_ASSERT_EQUALS(static_cast<int>(output.size()), 0);

}

/**
 * Tests some functions that can be tested without aborting/exiting the 
 * program.
 *
 * This test makes really no sense, it just tries to raise the test coverage ;)
 */
void
ApplicationTest::testMisc() {
    Application::initialize();
    Application::initialize();
    Application::finalize();   
}

#endif
