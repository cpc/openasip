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
 * @file HDBRegistryTest.hh 
 *
 * A test suite for HDBRegistry.
 * 
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
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
