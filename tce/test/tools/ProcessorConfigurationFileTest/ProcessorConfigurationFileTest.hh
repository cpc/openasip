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
 * @file ProcessorConfigurationFileTest.hh
 * 
 * A test suite for ProcessorConfigurationFile.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
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
