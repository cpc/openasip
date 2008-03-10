/**
 * @file FUGuardEncodingTest.hh
 *
 * A test suite for FUGuardEncoding.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef FUGuardEncodingTest_HH
#define FUGuardEncodingTest_HH

#include <string>

#include <TestSuite.h>
#include "BinaryEncoding.hh"
#include "FUGuardEncoding.hh"
#include "MoveSlot.hh"
#include "GuardField.hh"

using std::string;

/**
 * Test suite for testing FUGuardEncoding class.
 */
class FUGuardEncodingTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();

private:
    BinaryEncoding* bem_;
    GuardField* gField_;
};


/**
 * Called before each test.
 */
void
FUGuardEncodingTest::setUp() {
    bem_ = new BinaryEncoding();
    MoveSlot* slot = new MoveSlot("b1", *bem_);
    gField_ = new GuardField(*slot);
}


/**
 * Called after each test.
 */
void
FUGuardEncodingTest::tearDown() {
    delete bem_;
}


/**
 * Tests the constructor of FUGuardEncoding class.
 */
void
FUGuardEncodingTest::testCreation() {

    const string fu1 = "fu1";
    const string port1 = "port1";

    FUGuardEncoding* enc1 = 
	new FUGuardEncoding(fu1, port1, false, 0, *gField_);
    TS_ASSERT(gField_->hasFUGuardEncoding(fu1, port1, false));
    TS_ASSERT_THROWS(
	new FUGuardEncoding(fu1, port1, false, 1, *gField_), 
	ObjectAlreadyExists);
    TS_ASSERT_THROWS(
	new FUGuardEncoding(fu1, port1, true, 0, *gField_), 
	ObjectAlreadyExists);
    
    TS_ASSERT(enc1->functionUnit() == fu1);
    TS_ASSERT(enc1->port() == port1);
    TS_ASSERT(!enc1->isGuardInverted());
    TS_ASSERT(enc1->encoding() == 0);
    TS_ASSERT(enc1->parent() == gField_);
}

#endif
