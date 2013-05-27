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
 * @file IDFSerializerTest.hh 
 *
 * A test suite for IDFSerializer.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef IDFSerializerTest_HH
#define IDFSerializerTest_HH

#include <string>
#include <TestSuite.h>

#include "IDFSerializer.hh"
#include "MachineImplementation.hh"
#include "FileSystem.hh"
#include "ObjectState.hh"

using std::string;
using namespace IDF;

const string CURR_WORK_DIR = FileSystem::currentWorkingDir();
const string DS = FileSystem::DIRECTORY_SEPARATOR;
const string WORM_IDF = "data" + DS + "worm.idf";
const string TARGET_IDF = "data" + DS + "new.idf";
const string DECOMPRESSOR_FILE = FileSystem::currentWorkingDir() + DS + 
    "data" + DS + "decompressor.vhdl";
const string HDB_FILE = FileSystem::currentWorkingDir() + DS + "data" + DS +
    "icdecoderdata.hdb";
const string ICDECODER_PLUGIN_FILE = FileSystem::currentWorkingDir() + DS + 
    "data" + DS + "exampleIcDecoderPlugin.so";

/**
 * Tests different functionality of IDFSerializer class.
 */
class IDFSerializerTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    
    void testReadAndWriteState();
    void testRelativePaths();
    
private:
};


/**
 * Called before each test.
 */
void
IDFSerializerTest::setUp() {
}


/**
 * Called after each test.
 */
void
IDFSerializerTest::tearDown() {
}


/**
 * Tests the functionality of reading and writing IDF file.
 */
void
IDFSerializerTest::testReadAndWriteState() {

    IDFSerializer serializer;
    serializer.setSourceFile(WORM_IDF);

    ObjectState* implementationState = NULL;
    TS_ASSERT_THROWS_NOTHING(implementationState = serializer.readState());

    MachineImplementation* machImpl = NULL;
    TS_ASSERT_THROWS_NOTHING(
        machImpl = new MachineImplementation(implementationState));

    TS_ASSERT(machImpl->hasICDecoderPluginName());
    TS_ASSERT(machImpl->hasICDecoderPluginFile());
    TS_ASSERT(machImpl->hasICDecoderHDB());

    TS_ASSERT_EQUALS(static_cast<int>(machImpl->icDecoderParameterCount()), 2);
    TS_ASSERT_EQUALS(machImpl->icDecoderParameterName(0), "param1");
    TS_ASSERT_EQUALS(machImpl->icDecoderParameterValue(0), "Testi 1");
    TS_ASSERT_EQUALS(machImpl->icDecoderParameterName(1), "param2");
    TS_ASSERT_EQUALS(machImpl->icDecoderParameterValue(1), "123");

    TS_ASSERT(machImpl->hasDecompressorFile());
    TS_ASSERT_EQUALS(
        machImpl->icDecoderPluginName(), "exampleIcDecoderPlugin");
    TS_ASSERT_EQUALS(
        machImpl->icDecoderPluginFile(), ICDECODER_PLUGIN_FILE);
    TS_ASSERT_EQUALS(machImpl->icDecoderHDB(), HDB_FILE);
    TS_ASSERT_EQUALS(machImpl->decompressorFile(), DECOMPRESSOR_FILE);
    
    TS_ASSERT(machImpl->hasFUImplementation("LSU"));
    TS_ASSERT(machImpl->hasFUImplementation("ADD"));
    TS_ASSERT(!machImpl->hasFUImplementation("RF"));
    TS_ASSERT(machImpl->hasRFImplementation("RF"));
    TS_ASSERT(machImpl->hasBusImplementation("BUS1"));
    TS_ASSERT(machImpl->hasSocketImplementation("SOCKET1"));
    TS_ASSERT(!machImpl->hasBusImplementation("SOCKET_FOO"));

    serializer.setDestinationFile(TARGET_IDF);
    TS_ASSERT_THROWS_NOTHING(
        serializer.writeMachineImplementation(*machImpl));

    delete machImpl;

    serializer.setSourceFile(TARGET_IDF);
    TS_ASSERT_THROWS_NOTHING(implementationState = serializer.readState());  

    TS_ASSERT_THROWS_NOTHING(
        machImpl = new MachineImplementation(implementationState));

    TS_ASSERT(machImpl->hasICDecoderPluginName());
    TS_ASSERT(machImpl->hasICDecoderPluginFile());
    TS_ASSERT(machImpl->hasICDecoderHDB());
    TS_ASSERT(machImpl->hasDecompressorFile());
    TS_ASSERT_EQUALS(
        machImpl->icDecoderPluginName(), "exampleIcDecoderPlugin");
    TS_ASSERT_EQUALS(
        machImpl->icDecoderPluginFile(), ICDECODER_PLUGIN_FILE);
    TS_ASSERT_EQUALS(machImpl->icDecoderHDB(), HDB_FILE);
    TS_ASSERT_EQUALS(machImpl->decompressorFile(), DECOMPRESSOR_FILE);

    TS_ASSERT(machImpl->hasFUImplementation("LSU"));
    TS_ASSERT(machImpl->hasFUImplementation("ADD"));
    TS_ASSERT(!machImpl->hasFUImplementation("RF"));
    TS_ASSERT(machImpl->hasRFImplementation("RF"));
    TS_ASSERT(machImpl->hasBusImplementation("BUS1"));
    TS_ASSERT(machImpl->hasSocketImplementation("SOCKET1"));
    TS_ASSERT(!machImpl->hasBusImplementation("SOCKET_FOO"));
    
    delete implementationState;
    delete machImpl;
}

/**
 * Tests the functionality of forming relative paths out of absolute paths.
 */
void
IDFSerializerTest::testRelativePaths() {
        
    IDFSerializer serializer;
    serializer.setSourceFile(WORM_IDF);

    ObjectState* implementationState = NULL;
    TS_ASSERT_THROWS_NOTHING(implementationState = serializer.readState());

    MachineImplementation* machImpl = NULL;
    TS_ASSERT_THROWS_NOTHING(
        machImpl = new MachineImplementation(implementationState));

    TS_ASSERT(machImpl->hasICDecoderPluginFile());
    TS_ASSERT(machImpl->hasDecompressorFile());
    TS_ASSERT(machImpl->hasICDecoderHDB());
    
    std::vector<string> searchPaths;
    searchPaths.push_back(CURR_WORK_DIR);
        
    string relPath;
    string relPluginFile = "data" + DS + "exampleIcDecoderPlugin.so";
    string relDecompressorFile = "data" + DS + "decompressor.vhdl";
    string relDecoderHDBFile = "data" + DS + "icdecoderdata.hdb";

    // provide absolute path of every file for relative path search

    FileSystem::makeRelativePath(
        searchPaths, machImpl->icDecoderPluginFile(), relPath);
    TS_ASSERT_EQUALS(relPath, relPluginFile);
    
    FileSystem::makeRelativePath(
        searchPaths, machImpl->decompressorFile(), relPath);
    TS_ASSERT_EQUALS(relPath, relDecompressorFile);
    
    FileSystem::makeRelativePath(
        searchPaths, machImpl->icDecoderHDB(), relPath);
    TS_ASSERT_EQUALS(relPath, relDecoderHDBFile);
    
    delete implementationState;
    delete machImpl;
}

#endif
