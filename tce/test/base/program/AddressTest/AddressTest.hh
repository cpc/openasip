/** 
 * @file AddressTest.hh
 * 
 * A test suite for Address.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
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
