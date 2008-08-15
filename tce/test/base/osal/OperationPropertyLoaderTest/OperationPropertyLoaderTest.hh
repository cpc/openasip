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
 * @file OperationPropertyLoaderTest.hh
 * 
 * A test suite for OperationPropertyLoader.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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
