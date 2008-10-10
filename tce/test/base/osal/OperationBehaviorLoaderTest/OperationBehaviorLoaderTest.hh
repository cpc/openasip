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
 * @file OperationBehaviorLoaderTest.hh
 * 
 * A test suite for OperationBehaviorLoader.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 */

#ifndef OPERATION_BEHAVIOR_LOADER_TEST_HH
#define OPERATION_BEHAVIOR_LOADER_TEST_HH

#include <TestSuite.h>
#include <string>
#include <vector>
#include <iostream>

#include "OperationBehaviorLoader.hh"
#include "OperationContext.hh"
#include "OperationIndex.hh"
#include "FileSystem.hh"
#include "Exception.hh"
#include "Operation.hh"
#include "Application.hh"
#include "SimValue.hh"
#include "TCEString.hh"

using std::string;
using std::vector;
using std::cout;
using std::endl;

/**
 * Class for testing OperationBehaviorLoader.
 */
class OperationBehaviorLoaderTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testImportBehavior();
    void testErrors();

private:
    static const string PATH;
};

const string OperationBehaviorLoaderTest::PATH = 
FileSystem::currentWorkingDir() + FileSystem::DIRECTORY_SEPARATOR + "data";

/**
 * Called before each test.
 */
void
OperationBehaviorLoaderTest::setUp() {
}


/**
 * Called after each test.
 */
void
OperationBehaviorLoaderTest::tearDown() {
}

/**
 * Test that importBehavior() works.
 */
void
OperationBehaviorLoaderTest::testImportBehavior() {

    Operation oper("OPER1", NullOperationBehavior::instance());
    OperationIndex index;
    TS_ASSERT_THROWS_NOTHING(index.addPath(PATH));
    OperationBehaviorLoader loader(index); 
    OperationContext context;
    
    //vector<SimValue*> arguments;
    SimValue** arguments = new SimValue*[2];
    
    SimValue input1(32);
    input1 = 1;
    arguments[0] = &input1;
    
    SimValue result(32);
    arguments[1] = &result;

    OperationBehavior* behavior = NULL;
    TS_ASSERT_THROWS_NOTHING(behavior = &(loader.importBehavior(oper)));
    
    TS_ASSERT_EQUALS(behavior->simulateTrigger(arguments, context), true);
    TS_ASSERT_EQUALS(result.intValue(), 1 * 3);
    delete[] arguments;
    arguments = NULL;
}

/**
 * Test that erronous situation are handled with exceptions.
 */
void 
OperationBehaviorLoaderTest::testErrors() {
    Operation operation("FOO", NullOperationBehavior::instance());
    OperationIndex index;
    TS_ASSERT_THROWS_NOTHING(index.addPath(PATH));
    OperationBehaviorLoader loader(index);

    // trying to look non-existing operation
    TS_ASSERT_THROWS(
        loader.importBehavior(operation), DynamicLibraryException);
}

#endif
