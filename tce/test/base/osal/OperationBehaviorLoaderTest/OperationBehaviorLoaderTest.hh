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
#include "OperationBehavior.hh"

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
