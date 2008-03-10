/** 
 * @file HDBRegistryTest.hh 
 *
 * A test suite for HDBRegistry.
 * 
 * @author Jari Mäntyneva 2006 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDB_REGISTRY_TEST_HH
#define TTA_HDB_REGISTRY_TEST_HH

#include <string>
#include <TestSuite.h>

#include "FileSystem.hh"
#include "HDBRegistry.hh"
#include "HDBManager.hh"
#include "Exception.hh"

#include <iostream>
using std::cerr;
using std::endl;
using std::string;
using namespace HDB;

const string DS = FileSystem::DIRECTORY_SEPARATOR;
const string HDB_TO_LOAD = "data" + DS + "testHDB.hdb";

/**
 * Class that tests HDBRegistry class.
 */
class HDBRegistryTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testLoadingHDB();

private:

};


/**
 * Called before each test.
 */
void
HDBRegistryTest::setUp() {
}


/**
 * Called after each test.
 */
void
HDBRegistryTest::tearDown() {
}


/**
 * Tests creating loading HDBs.
 */
void
HDBRegistryTest::testLoadingHDB() {

    HDBRegistry& registry = HDBRegistry::instance();

    int hdbCount = registry.hdbCount();
    TS_ASSERT_EQUALS(hdbCount, 0);
    TS_ASSERT_THROWS_NOTHING(registry.hdb(HDB_TO_LOAD));
    hdbCount = registry.hdbCount();
    TS_ASSERT_DIFFERS(hdbCount, 0);
    TS_ASSERT_THROWS_NOTHING(registry.hdb(hdbCount - 1));
    TS_ASSERT_THROWS(registry.hdb(hdbCount), OutOfRange);
    registry.loadFromSearchPaths();

    // new hdbCount should differ from the old one
    TS_ASSERT_DIFFERS(hdbCount, registry.hdbCount());
}    

#endif
