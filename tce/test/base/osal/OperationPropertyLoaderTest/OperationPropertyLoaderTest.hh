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
 * @file OperationPropertyLoaderTest.hh
 * 
 * A test suite for OperationPropertyLoader.
 *
 * @author Jussi Nyk�nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef OPERATION_PROPERTY_LOADER_TEST_HH
#define OPERATION_PROPERTY_LOADER_TEST_HH

#include <TestSuite.h>
#include <string>

#include "OperationPropertyLoader.hh"
#include "OperationBehavior.hh"
#include "Operation.hh"
#include "OperationModule.hh"
#include "TCEString.hh"

using std::string;

/**
 * Class that test OperationPropertyLoader.
 */
class OperationPropertyLoaderTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testLoadOperationProperties();

private:
    /// Name of the correct XML file.
    static const string FILE_NAME;
    /// Name of the erronous XML file.
    static const string BAD_FILE;
    /// Path of the XML files.
    static const string PATH;
};

const string OperationPropertyLoaderTest::FILE_NAME = "correct";
const string OperationPropertyLoaderTest::BAD_FILE = "erronous";
const string OperationPropertyLoaderTest::PATH = 
FileSystem::currentWorkingDir() + FileSystem::DIRECTORY_SEPARATOR + "data";

/**
 * Called before each test.
 */
void
OperationPropertyLoaderTest::setUp() {
}


/**
 * Called after each test.
 */
void
OperationPropertyLoaderTest::tearDown() {
}

/**
 * Test that loading Operation properties works.
 */
void
OperationPropertyLoaderTest::testLoadOperationProperties() {
    OperationPropertyLoader loader;
    OperationModule module(FILE_NAME, PATH);
    OperationModule badModule(BAD_FILE, PATH);
    Operation oper("OPER1", NullOperationBehavior::instance());
    Operation notExists("foo", NullOperationBehavior::instance());
    
    TS_ASSERT_THROWS_NOTHING(loader.loadOperationProperties(oper, module));
    TS_ASSERT_THROWS(loader.loadOperationProperties(notExists, module),
                     InstanceNotFound);

    TS_ASSERT_THROWS(loader.loadOperationProperties(oper, badModule),
                     InstanceNotFound);
    
    TS_ASSERT_EQUALS(oper.numberOfInputs(), 2);
    TS_ASSERT_EQUALS(oper.numberOfOutputs(), 1);
    TS_ASSERT_EQUALS(oper.readsMemory(), true);
}

#endif
