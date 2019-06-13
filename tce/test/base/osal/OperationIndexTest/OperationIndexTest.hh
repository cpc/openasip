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
 * @file OperationIndexTest.hh
 * 
 * A test suite for OperationIndex.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef OPERATION_INDEX_TEST_HH
#define OPERATION_INDEX_TEST_HH

#include <TestSuite.h>
#include <string>

#include "OperationIndex.hh"
#include "FileSystem.hh"
#include "OperationModule.hh"

using std::string;

class OperationIndexTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testPathsAndModules();
    void testOperations();
    void testAddAndRemoveModule();

private:
    /// Path where modules are searched.
    static const string SEARCH_PATH;
    /// Non-existing path.
    static const string NON_EXISTING_PATH;
};

const string OperationIndexTest::SEARCH_PATH = 
FileSystem::currentWorkingDir() + FileSystem::DIRECTORY_SEPARATOR + "data";
const string OperationIndexTest::NON_EXISTING_PATH =
FileSystem::DIRECTORY_SEPARATOR + "bar" + FileSystem::DIRECTORY_SEPARATOR +
"foo" + FileSystem::DIRECTORY_SEPARATOR + "bar";

/**
 * Called before each test.
 */
void
OperationIndexTest::setUp() {
}


/**
 * Called after each test.
 */
void
OperationIndexTest::tearDown() {
}

/**
 * Test that module and path methods work.
 */
void
OperationIndexTest::testPathsAndModules() {
    OperationIndex index;
    TS_ASSERT_THROWS_NOTHING(index.addPath(SEARCH_PATH));
    // adding the same path twice is not an error
    TS_ASSERT_THROWS_NOTHING(index.addPath(SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(index.addPath(NON_EXISTING_PATH));
    string path;
    
    TS_ASSERT_THROWS_NOTHING(path = index.path(0));
    TS_ASSERT_EQUALS(path, SEARCH_PATH);
    
    TS_ASSERT_EQUALS(index.pathCount(), 2);

    OperationModule& mod = index.moduleOf("oper1");
    TS_ASSERT_EQUALS(mod.name(), "correct");
    
    OperationModule& module = index.module(0);
    TS_ASSERT_EQUALS(module.name(), "correct");

    OperationModule& module2 = index.module(0, SEARCH_PATH);
    TS_ASSERT_EQUALS(module2.name(), "correct");

    TS_ASSERT_THROWS(index.module(5, SEARCH_PATH),
                     OutOfRange);
    TS_ASSERT_THROWS(
        index.module(0, NON_EXISTING_PATH), OutOfRange);
   
    string fooPath = "/foo/bar/foo";
    TS_ASSERT_THROWS(index.module(0, fooPath), PathNotFound);
    TS_ASSERT_THROWS(index.moduleCount(fooPath), PathNotFound);

    TS_ASSERT_EQUALS(index.moduleCount(), 1);
    module = index.module(0, SEARCH_PATH);
    TS_ASSERT_EQUALS(module.name(), "correct");
    
    int moduleCount = 1;
    TS_ASSERT_THROWS_NOTHING(moduleCount = index.moduleCount(SEARCH_PATH));
    TS_ASSERT_EQUALS(moduleCount, 1);

    TS_ASSERT_EQUALS(index.moduleCount(NON_EXISTING_PATH), 0);
}


/**
 * Test that operation methods work.
 */
void
OperationIndexTest::testOperations() {

    OperationIndex index;
    TS_ASSERT_THROWS_NOTHING(index.addPath(SEARCH_PATH));
        
    OperationModule module("correct", SEARCH_PATH);
    OperationModule nonExistingMod("foo", SEARCH_PATH);
    OperationModule& modRef = module;
    string name = "";
    TS_ASSERT_THROWS_NOTHING(name = index.operationName(0, modRef));

    int opCount = 1;
    TS_ASSERT_THROWS_NOTHING(opCount = index.operationCount(module));
    TS_ASSERT_EQUALS(opCount, 1);
    TS_ASSERT_THROWS(index.operationCount(nonExistingMod), BadOperationModule);
      
    TS_ASSERT_THROWS_NOTHING(modRef = index.moduleOf("oper1"));
    
    TS_ASSERT_EQUALS(modRef.name(), "correct");
}

/**
 * Test adding new module.
 */
void
OperationIndexTest::testAddAndRemoveModule() {
	
    OperationIndex index;
    TS_ASSERT_THROWS_NOTHING(index.addPath(SEARCH_PATH));
    
    TS_ASSERT_EQUALS(index.moduleCount(), 1);
    
    TS_ASSERT_EQUALS(index.module(0, SEARCH_PATH).name(), "correct");
    OperationModule* module = new OperationModule("foo", SEARCH_PATH);
    
    TS_ASSERT_THROWS_NOTHING(index.addModule(module, SEARCH_PATH));
    TS_ASSERT_THROWS_NOTHING(index.refreshModule(SEARCH_PATH, "foo"));

    // trying to refresh unexisting module
    TS_ASSERT_THROWS(index.refreshModule(NON_EXISTING_PATH, "daa"), PathNotFound);

    TS_ASSERT_THROWS(index.addModule(module, NON_EXISTING_PATH), PathNotFound);

    TS_ASSERT_EQUALS(index.moduleCount(), 2);
    TS_ASSERT_EQUALS(index.moduleCount(SEARCH_PATH), 2);
    
    TS_ASSERT_EQUALS(index.module(0, SEARCH_PATH).name(), "correct");
    TS_ASSERT_EQUALS(index.module(1, SEARCH_PATH).name(), "foo");
    
    TS_ASSERT_THROWS_NOTHING(index.removeModule(SEARCH_PATH, "foo"));
    TS_ASSERT_THROWS(index.removeModule(SEARCH_PATH, "daa"), InstanceNotFound);
    TS_ASSERT_THROWS(index.removeModule(NON_EXISTING_PATH, "foo"), PathNotFound);
   
    TS_ASSERT_EQUALS(index.moduleCount(), 1);
    TS_ASSERT_EQUALS(index.moduleCount(SEARCH_PATH), 1);
    TS_ASSERT_EQUALS(index.module(0, SEARCH_PATH).name(), "correct");

    TS_ASSERT_THROWS_NOTHING(index.removeModule(SEARCH_PATH, "correct"));
    TS_ASSERT_EQUALS(index.moduleCount(), 0);
    TS_ASSERT_EQUALS(index.moduleCount(SEARCH_PATH), 0);
}

#endif
