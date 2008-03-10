/** 
 * @file RegisterFileTest.hh 
 *
 * A test suite for RegisterFile class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef RegisterFileTest_HH
#define RegisterFileTest_HH

#include <string>

#include <TestSuite.h>
#include "Port.hh"
#include "Segment.hh"
#include "ADFSerializer.hh"
#include "FileSystem.hh"

using std::string;
using namespace TTAMachine;

/**
 * Tests the functionality of RegisterFile class.
 */
class RegisterFileTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testSetName();
    void testSetNumberOfRegisters();
    void testSetWidth();
    void testLoadState();
    void testObjectStateLoadingErrors();
};


/**
 * Called before each test.
 */
void
RegisterFileTest::setUp() {
}


/**
 * Called after each test.
 */
void
RegisterFileTest::tearDown() {
}


/**
 * Tests setting the name of the register file.
 */
void
RegisterFileTest::testSetName() {

    const string rf1Name = "rf1";
    const string rf2Name = "rf2";
    const string otherName = "foo";
    
    Machine mach;
    RegisterFile* rf1 = new RegisterFile(
        rf1Name, 30, 32, 1, 1, 0, RegisterFile::NORMAL);
    TS_ASSERT(rf1->name() == rf1Name);
    TS_ASSERT_THROWS_NOTHING(rf1->setName(rf1Name));
    TS_ASSERT_THROWS_NOTHING(rf1->setName(rf2Name));
    TS_ASSERT(rf1->name() == rf2Name);
    rf1->setName(rf1Name);
    
    mach.addRegisterFile(*rf1);

    RegisterFile* rf2 = new RegisterFile(
        rf2Name, 30, 32, 1, 1, 0, RegisterFile::NORMAL);
    mach.addRegisterFile(*rf2);

    TS_ASSERT_THROWS(rf2->setName(rf1Name), ComponentAlreadyExists);
    TS_ASSERT(rf2->name() == rf2Name);
    TS_ASSERT_THROWS_NOTHING(rf2->setName(otherName));
    TS_ASSERT(rf2->name() == otherName);
}


/**
 * Tests setting the number of registers.
 */
void
RegisterFileTest::testSetNumberOfRegisters() {
    RegisterFile rf1("rf1", 30, 32, 1, 1, 0, RegisterFile::NORMAL);
    TS_ASSERT_THROWS(rf1.setNumberOfRegisters(0), OutOfRange);
    TS_ASSERT(rf1.numberOfRegisters() == 30);
    TS_ASSERT_THROWS_NOTHING(rf1.setNumberOfRegisters(20));
    TS_ASSERT(rf1.numberOfRegisters() == 20);
}


/**
 * Tests setting the width of the register file.
 */
void
RegisterFileTest::testSetWidth() {
    RegisterFile rf1("rf1", 30, 32, 1, 1, 0, RegisterFile::NORMAL);
    TS_ASSERT_THROWS(rf1.setWidth(0), OutOfRange);
    TS_ASSERT(rf1.width() == 32);
    TS_ASSERT_THROWS_NOTHING(rf1.setWidth(16));
    TS_ASSERT(rf1.width() == 16);
}
   

/**
 * Tests saving the state of register file to an ObjectState tree and
 * loading the state from it.
 */
void
RegisterFileTest::testLoadState() {

    const string rf1Name = "rf1";

    RegisterFile rf(rf1Name, 30, 32, 0, 0, 0, RegisterFile::NORMAL);
    ObjectState* rf1State = rf.saveState();

    RegisterFile loaded(rf1State);
    TS_ASSERT(loaded.name() == rf1Name);
    TS_ASSERT(loaded.numberOfRegisters() == 30);
    TS_ASSERT(loaded.width() == 32);
    TS_ASSERT(loaded.maxReads() == 0);
    TS_ASSERT(loaded.maxWrites() == 0);
    TS_ASSERT(loaded.type() == RegisterFile::NORMAL);

    delete rf1State;
    rf.setType(RegisterFile::RESERVED);
    rf1State = rf.saveState();
    TS_ASSERT_THROWS_NOTHING(loaded.loadState(rf1State));
    TS_ASSERT(loaded.type() == RegisterFile::RESERVED);

    delete rf1State;
    rf.setType(RegisterFile::VOLATILE);
    rf1State = rf.saveState();
    TS_ASSERT_THROWS_NOTHING(loaded.loadState(rf1State));
    TS_ASSERT(loaded.type() == RegisterFile::VOLATILE);

    rf1State->setAttribute(RegisterFile::OSKEY_TYPE, "unknownType");
    TS_ASSERT_THROWS(
        loaded.loadState(rf1State), ObjectStateLoadingException);
    delete rf1State;
}
   

/**
 * Tests creating register file from an erronous mdf file.
 */
void
RegisterFileTest::testObjectStateLoadingErrors() {

    const string DIR_SEP = FileSystem::DIRECTORY_SEPARATOR;
    const string PORTS_CONNECTED_TO_SAME_SOCKET = "." + DIR_SEP + "data" +
        DIR_SEP + "PortsConnectedToSameSocket.mdf";
    const string INVALID_SOCKET_NAME = "." + DIR_SEP + "data" + DIR_SEP +
        "InvalidSocketName.mdf";
    const string SAME_SOCKET_DIRECTION = "." + DIR_SEP + "data" + DIR_SEP +
        "SameSocketDirection.mdf";
    const string VALID = "." + DIR_SEP + "data" + DIR_SEP + "Valid.mdf";

    ADFSerializer serializer;
    Machine mach;

    serializer.setSourceFile(PORTS_CONNECTED_TO_SAME_SOCKET);
    ObjectState* machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;
    
    serializer.setSourceFile(INVALID_SOCKET_NAME);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(SAME_SOCKET_DIRECTION);
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(VALID);
    machState = serializer.readState();
    TS_ASSERT_THROWS_NOTHING(mach.loadState(machState));
    delete machState;    
}

#endif
