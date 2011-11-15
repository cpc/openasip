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
 * @file EnvironmentTest.hh 
 *
 * A test suite for Environment.
 * 
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note reviewed 2 June 2004 by jm, pj, tr, jn
 * @note rating: green
 */

#ifndef TTA_ENVIRONMENT_TEST_HH
#define TTA_ENVIRONMENT_TEST_HH

#include <TestSuite.h>
#include <string>

#include "Environment.hh"
#include "FileSystem.hh"
#include "tce_config.h"

using std::string;


/**
 * A test class for Environment class.
 */
class EnvironmentTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();
    
    void testFindDirs();
    void testNotFinding();
    void testNewDirs();
    
private:
    /// Name of an application.
    static const string APPLICATION_NAME;
    /// An invalid application name.
    static const string INVALID_NAME;
    /// Empty string.
    static const string EMPTY_STRING;
};


// initializing static member variables
const string EnvironmentTest::APPLICATION_NAME = "ProDe";
const string EnvironmentTest::INVALID_NAME = "koala bear";
const string EnvironmentTest::EMPTY_STRING = "";


/**
 * Called before each test.
 */
void
EnvironmentTest::setUp() {
}


/**
 * Called after each test.
 */
void
EnvironmentTest::tearDown() {
}


/**
 * Tests that schema directory, data directory, and bitmaps directory are
 * found.
 */
void
EnvironmentTest::testFindDirs() {

    string DS = FileSystem::DIRECTORY_SEPARATOR;
    
    string path = Environment::dataDirPath(APPLICATION_NAME);
    string correctPath = string(TCE_SRC_ROOT) + DS + "data" + DS + "ProDe";
    TS_ASSERT_EQUALS(path, correctPath);

    path = Environment::bitmapsDirPath(APPLICATION_NAME);
    correctPath = string(TCE_SRC_ROOT) + DS + "data" + DS +
    		"bitmaps" + DS + "ProDe";
    TS_ASSERT_EQUALS(path, correctPath);

    path = Environment::iconDirPath();
    correctPath = string(TCE_SRC_ROOT) + DS + "data" + DS + "icons";
    TS_ASSERT_EQUALS(path, correctPath);

    TCEString userConf = 
        FileSystem::homeDirectory() + DS + ".tce" + DS + "ProDe.conf";

    path = Environment::confPath("ProDe.conf");
    TCEString srcConf = string(TCE_SRC_ROOT) + DS + "conf" + DS + "ProDe.conf";
    TS_ASSERT(path == userConf || path == srcConf);

    path = Environment::schemaDirPath(APPLICATION_NAME);
    correctPath = string(TCE_SRC_ROOT) + DS + "data" + DS + "ProDe";
    TS_ASSERT_EQUALS(path, correctPath);    
}


/**
 * Tests that when no directory path is found, an empry string is returned.
 */
void
EnvironmentTest::testNotFinding() {

    string returnedString = Environment::dataDirPath(INVALID_NAME);
    TS_ASSERT_EQUALS(returnedString, EMPTY_STRING);

    returnedString = Environment::schemaDirPath(INVALID_NAME);
    TS_ASSERT_EQUALS(returnedString, EMPTY_STRING);

    returnedString = Environment::bitmapsDirPath(INVALID_NAME);
    TS_ASSERT_EQUALS(returnedString, EMPTY_STRING);

    returnedString = Environment::manDirPath(INVALID_NAME);
    TS_ASSERT_EQUALS(returnedString, EMPTY_STRING);
   
    // error log file is not currently found
    returnedString = Environment::errorLogFilePath();
    TS_ASSERT_EQUALS(returnedString, EMPTY_STRING);
}


/**
 * Test that if directory is not found, a new directory is created.
 */
void
EnvironmentTest::testNewDirs() {
    string DS = FileSystem::DIRECTORY_SEPARATOR;

    string schemaDir = FileSystem::currentWorkingDir() + DS + "data" + DS +
	"fooSchema";
    string dataDir = FileSystem::currentWorkingDir() + DS + "data" + DS +
	"fooData";
    string bitmapDir = FileSystem::currentWorkingDir() + DS + "data" + DS +
	"fooBitmap";
    string manDir = FileSystem::currentWorkingDir() + DS + "data" + DS +
	"fooMan";
	string errorDir = FileSystem::currentWorkingDir() + DS + "data" + DS +
	"fooError";

    Environment::setNewSchemaFileDir(schemaDir);
    Environment::setNewDataFileDir(dataDir);
    Environment::setNewBitmapsFileDir(bitmapDir);
    Environment::setNewManFileDir(manDir);
	Environment::setNewErrorLogFileDir(errorDir);
    
    string retString = Environment::dataDirPath(INVALID_NAME);
    TS_ASSERT_EQUALS(retString, dataDir);
    
    retString = Environment::schemaDirPath(INVALID_NAME);
    TS_ASSERT_EQUALS(retString, schemaDir);
    
    retString = Environment::bitmapsDirPath(INVALID_NAME);
    TS_ASSERT_EQUALS(retString, bitmapDir);
    
    retString = Environment::manDirPath(INVALID_NAME);
    TS_ASSERT_EQUALS(retString, manDir);
    
        retString = Environment::errorLogFilePath();
    TS_ASSERT_EQUALS(retString, errorDir + DS + "error_log.log");

    // destroy all created directories
    FileSystem::removeFileOrDirectory(schemaDir);
    FileSystem::removeFileOrDirectory(dataDir);
    FileSystem::removeFileOrDirectory(bitmapDir);
    FileSystem::removeFileOrDirectory(manDir);
	FileSystem::removeFileOrDirectory(errorDir);
    
}

#endif
