/*
    Copyright (c) 2002-2009 Tampere University.

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
