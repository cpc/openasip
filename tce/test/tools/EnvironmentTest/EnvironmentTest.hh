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
 * @file EnvironmentTest.hh 
 *
 * A test suite for Environment.
 * 
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
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

    path = Environment::confDirPath("ProDe.conf");
    correctPath = string(TCE_SRC_ROOT) + DS + "conf" + DS + "ProDe.conf";
    TS_ASSERT_EQUALS(path, correctPath);

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

    returnedString = Environment::confDirPath(INVALID_NAME);
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
