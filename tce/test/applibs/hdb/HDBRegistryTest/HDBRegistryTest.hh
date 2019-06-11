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
