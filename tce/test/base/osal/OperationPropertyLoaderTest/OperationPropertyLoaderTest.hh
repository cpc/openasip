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
