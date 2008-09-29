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
 * @file ConfigurationFileTest.hh
 * 
 * A test suite for ConfigurationFile.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 */

#ifndef CONFIGURATION_FILE_TEST_HH
#define CONFIGURATION_FILE_TEST_HH

#include <TestSuite.h>
#include <string>
#include <fstream>

#include "ConfigurationFile.hh"

using std::string;
using std::ifstream;

/**
 * Test class for ConfigurationFile class.
 */
class ConfigurationFileTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testLoad();
private:
    static const string FILE;
};

const string ConfigurationFileTest::FILE = "data/testfile.conf";

/**
 * Called before each test.
 */
void
ConfigurationFileTest::setUp() {
}


/**
 * Called after each test.
 */
void
ConfigurationFileTest::tearDown() {
}

/**
 * Test that reading a configuration file works.
 */
void
ConfigurationFileTest::testLoad() {

    ifstream inputStream(FILE.c_str());
    ConfigurationFile conf;
    conf.load(inputStream);
    inputStream.close();

    TS_ASSERT_EQUALS(conf.itemCount("foo"), 2);
    TS_ASSERT_EQUALS(conf.itemCount("bar"), 2);
    TS_ASSERT_EQUALS(conf.itemCount("integer1"), 2);
    TS_ASSERT_EQUALS(conf.itemCount("integer2"), 3);
    TS_ASSERT_EQUALS(conf.itemCount("float1"), 2);
    TS_ASSERT_EQUALS(conf.itemCount("float2"), 3);
    TS_ASSERT_EQUALS(conf.itemCount("boolean"), 2);

    TS_ASSERT_EQUALS(conf.stringValue("foo", 0), "jussi");
    TS_ASSERT_EQUALS(conf.stringValue("foo", 1), "pekka");

    TS_ASSERT_EQUALS(conf.stringValue("bar", 0), "jussi");
    TS_ASSERT_EQUALS(conf.stringValue("bar", 1), "pekka");

    TS_ASSERT_EQUALS(conf.intValue("integer1", 0), 1);
    TS_ASSERT_EQUALS(conf.intValue("integer1", 1), 2);
    
    TS_ASSERT_EQUALS(conf.intValue("integer2", 0), 3);
    TS_ASSERT_EQUALS(conf.intValue("integer2", 1), 4);
    TS_ASSERT_EQUALS(conf.intValue("integer2", 2), 5);

    TS_ASSERT_DELTA(conf.floatValue("float1", 0), 23.23, 0.001);
    TS_ASSERT_DELTA(conf.floatValue("float1", 1), 32.42, 0.001);
    
    TS_ASSERT_DELTA(conf.floatValue("float2", 0), 43.43, 0.001);
    TS_ASSERT_DELTA(conf.floatValue("float2", 1), 54.54, 0.001);
    TS_ASSERT_DELTA(conf.floatValue("float2", 2), 65.54, 0.001);

    TS_ASSERT_EQUALS(conf.booleanValue("boolean", 0), true);
    TS_ASSERT_EQUALS(conf.booleanValue("boolean", 1), false);
}

#endif
