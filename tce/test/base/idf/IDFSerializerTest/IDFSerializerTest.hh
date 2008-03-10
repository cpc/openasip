/** 
 * @file IDFSerializerTest.hh 
 *
 * A test suite for IDFSerializer.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef IDFSerializerTest_HH
#define IDFSerializerTest_HH

#include <string>
#include <TestSuite.h>

#include "IDFSerializer.hh"
#include "MachineImplementation.hh"
#include "FileSystem.hh"

using std::string;
using namespace IDF;

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

#endif
