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
