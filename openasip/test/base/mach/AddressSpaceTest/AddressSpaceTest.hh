/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file AddressSpaceTest.hh
 *
 * A test suite for AddressSpace.
 *
 * @author Andrea Cilio 2003 (a.cilio-no.spam-et.tudelft.nl)
 * @author Pekka Jääskeläinen 2010
 */

#ifndef AddressSpaceTest_HH
#define AddressSpaceTest_HH

#include <string>
#include <TestSuite.h>

#include "Machine.hh"
#include "AddressSpace.hh"
#include "ADFSerializer.hh"
#include "FileSystem.hh"
#include "Conversion.hh"
#include "ObjectState.hh"

using std::string;
using namespace TTAMachine;

/**
 * Test suite for testing AddressSpace class.
 */
class AddressSpaceTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testAddAndDeleteAddressSpace();
    void testLoadState();
    void testObjectStateLoadingErrors();

private:
    /// AddressSpace used in tests is stored here. 
    AddressSpace* asMemA_;
    /// AddressSpace used in tests is stored here.
    AddressSpace* asMemB_;
};


/**
 * Called before each test.
 */
void
AddressSpaceTest::setUp() {
    asMemA_ = 0;
    asMemB_ = 0;
}


/**
 * Called after each test.
 */
void
AddressSpaceTest::tearDown() {
}


/**
 * Tests functionality of adding an AddressSpace to machine and deleting it.
 */
void
AddressSpaceTest::testAddAndDeleteAddressSpace() {

    const string nameMemA = "memory_A";
    const string nameMemB = "memory_B";
    const int wdA = 32;
    const int wdB = 128;
    const int addrBegA = 1;
    const int addrBegB = 1024;
    const int addrEndA = 65535;
    const int addrEndB = 16383;

    Machine machine;

    asMemA_ = new AddressSpace(
        nameMemA, wdA, addrBegA, addrEndA, machine);
    asMemB_ = new AddressSpace(
        nameMemB, wdB, addrBegB, addrEndB, machine);

    Machine::AddressSpaceNavigator navigator = 
        machine.addressSpaceNavigator(); 
    AddressSpace* foundAsMemA = navigator.item(nameMemA);
    TS_ASSERT_EQUALS(foundAsMemA, asMemA_);

    AddressSpace* foundAsMemB = navigator.item(nameMemB);
    TS_ASSERT_EQUALS(foundAsMemB, asMemB_);

    const string nameNoMem = "memory_unexisting";
    TS_ASSERT(!navigator.hasItem(nameNoMem));

    machine.deleteAddressSpace(*asMemA_);
    TS_ASSERT(!navigator.hasItem(nameMemA));

    machine.deleteAddressSpace(*asMemB_);
    TS_ASSERT(!navigator.hasItem(nameMemB));

    asMemA_ = new AddressSpace(
        nameMemA, wdA, addrBegA, addrEndA, machine);
    TS_ASSERT_THROWS(
        new AddressSpace(nameMemA, wdB, addrBegB, addrEndB, machine),
        ComponentAlreadyExists);

    TS_ASSERT_THROWS(
        new AddressSpace(nameNoMem, wdA, 32, 31, machine),
        OutOfRange);
}


/**
 * Tests loading the state of address space from an ObjectState object.
 */
void
AddressSpaceTest::testLoadState() {

    const string as1Name = "as1";
    const string as2Name = "as2";
    const int width = 32;
    const unsigned int minAddr = 1;
    const unsigned int maxAddr = 100;

    Machine machine;
    AddressSpace* as1 = new AddressSpace(
        as1Name, width, minAddr, maxAddr, machine);
    ObjectState* asState = as1->saveState();
    
    TS_ASSERT_THROWS(
        new AddressSpace(asState, machine), ObjectStateLoadingException); 
    
    as1->setName(as2Name);
    AddressSpace* as2 = NULL;
    TS_ASSERT_THROWS_NOTHING(as2 = new AddressSpace(asState, machine));

    TS_ASSERT(as2->name() == as1Name);
    TS_ASSERT(as2->width() == width);
    TS_ASSERT(as2->start() == minAddr);
    TS_ASSERT(as2->end() == maxAddr);

    TS_ASSERT_THROWS(as1->loadState(asState), ObjectStateLoadingException);
    delete as2;

    TS_ASSERT_THROWS_NOTHING(as1->loadState(asState));
    delete asState;
}
   

/**
 * Tests loading the address space from an erroneous mdf file.
 */
void
AddressSpaceTest::testObjectStateLoadingErrors() {

    const string DIR_SEP = FileSystem::DIRECTORY_SEPARATOR;
    const string INVALID_MIN_ADDRESS = "." + DIR_SEP + "data" + DIR_SEP +
        "InvalidMinAddress.mdf";
    const string VALID = "." + DIR_SEP + "data" + DIR_SEP + "Valid.mdf";
    
    ADFSerializer serializer;
    Machine mach;

    serializer.setSourceFile(INVALID_MIN_ADDRESS);
    ObjectState* machState = NULL;
    machState = serializer.readState();
    TS_ASSERT_THROWS(mach.loadState(machState), ObjectStateLoadingException);
    delete machState;

    serializer.setSourceFile(VALID);
    CATCH_ANY(machState = serializer.readState());
    TS_ASSERT_THROWS_NOTHING(mach.loadState(machState));

    AddressSpace& as = *mach.addressSpaceNavigator().item(0);

    TS_ASSERT(as.hasNumericalId(0));
    TS_ASSERT(!as.hasNumericalId(1));
    TS_ASSERT(as.hasNumericalId(2));
    TS_ASSERT(as.isShared());
    
    delete machState;
}
                             

#endif
