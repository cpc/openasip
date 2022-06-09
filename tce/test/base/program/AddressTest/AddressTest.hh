/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file AddressTest.hh
 * 
 * A test suite for Address.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef ADDRESS_TEST_HH
#define ADDRESS_TEST_HH

#include <TestSuite.h>
#include "Address.hh"
#include "AddressSpace.hh"
#include "BaseType.hh"
#include "Machine.hh"

using namespace TTAMachine;
using namespace TTAProgram;

class AddressTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testAddress();

private:
};


/**
 * Called before each test.
 */
void
AddressTest::setUp() {
}


/**
 * Called after each test.
 */
void
AddressTest::tearDown() {
}

/**
 * Tests that Address works properly.
 */
void
AddressTest::testAddress() {
    Machine dummy_machine;
    AddressSpace as("AS", 16, 0, 1000, dummy_machine);

    UIntWord loc = 0xFF;

    Address address(loc, as);

    TS_ASSERT_EQUALS(&address.space(), &as);
    TS_ASSERT_EQUALS(address.location(), loc);
}

#endif
