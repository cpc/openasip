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
 * @file ProcessorConfigurationFileTest.hh
 * 
 * A test suite for ProcessorConfigurationFile.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 */

#ifndef PROCESSOR_CONFIGURATION_FILE_TEST_HH
#define PROCESSOR_CONFIGURATION_FILE_TEST_HH

#include <TestSuite.h>
#include <string>
#include <fstream>
#include <iostream>

#include "ProcessorConfigurationFile.hh"
#include "FileSystem.hh"

using std::string;
using std::ifstream;
using std::cout;
using std::endl;

/**
 * Test class for ProcessorConfigurationFile class.
 */
class ProcessorConfigurationFileTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testLoad();
    void testErrors();
private:
    /// Correct configuration file.
    static const string CORRECT_FILE;
    /// Erronous configuration file.
    static const string ERROR_FILE;
};

const string ProcessorConfigurationFileTest::CORRECT_FILE = "data/correct.conf";
const string ProcessorConfigurationFileTest::ERROR_FILE = "data/erronous.conf";

/**
 * Called before each test.
 */
void
ProcessorConfigurationFileTest::setUp() {
}


/**
 * Called after each test.
 */
void
ProcessorConfigurationFileTest::tearDown() {
}

/**
 * Test that loading the processor configuration file works.
 */
void
ProcessorConfigurationFileTest::testLoad() {

    ifstream inputStream(CORRECT_FILE.c_str());
    ProcessorConfigurationFile conf(inputStream);
    inputStream.close();

    conf.setPCFDirectory(FileSystem::directoryOfPath(CORRECT_FILE));

    TS_ASSERT_EQUALS(conf.errors(), false);
       
    TS_ASSERT_EQUALS(conf.architectureName(), "data/foo.adf");
    TS_ASSERT_EQUALS(static_cast<int>(conf.architectureSize()), 1238943);
    
    TS_ASSERT_EQUALS(conf.implementationName(), "data/foo.idf");
    TS_ASSERT_EQUALS(static_cast<int>(conf.implementationSize()), 324354);

    TS_ASSERT_EQUALS(conf.encodingMapName(), "data/foo.bem");
    TS_ASSERT_EQUALS(static_cast<int>(conf.encodingMapSize()), 129834);
}

/**
 * Test that errors are found while reading the configuration file.
 */
void
ProcessorConfigurationFileTest::testErrors() {

    ifstream inputStream(ERROR_FILE.c_str());
    ProcessorConfigurationFile conf(inputStream);
    inputStream.close();

    TS_ASSERT_EQUALS(conf.errors(), true);
    TS_ASSERT_EQUALS(conf.errorCount(), 4);
}

#endif
