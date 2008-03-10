/** 
 * @file ConfigurationFileTest.hh
 * 
 * A test suite for ConfigurationFile.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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
