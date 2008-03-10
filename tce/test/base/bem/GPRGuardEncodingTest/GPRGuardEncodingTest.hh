/**
 * @file GPRGuardEncodingTest.hh
 *
 * A test suite for GPRGuardEncoding.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef GPRGuardEncodingTest_HH
#define GPRGuardEncodingTest_HH

#include <string>

#include <TestSuite.h>
#include "BinaryEncoding.hh"
#include "GPRGuardEncoding.hh"
#include "MoveSlot.hh"
#include "GuardField.hh"

using std::string;

/**
 * Test suite for testing GPRGuardEncoding class.
 */
class GPRGuardEncodingTest : public CxxTest::TestSuite {
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
GPRGuardEncodingTest::setUp() {
    bem_ = new BinaryEncoding();
    MoveSlot* slot = new MoveSlot("b1", *bem_);
    gField_ = new GuardField(*slot);
}


/**
 * Called after each test.
 */
void
GPRGuardEncodingTest::tearDown() {
    delete bem_;
}


/**
 * Tests the constructor of GPRGuardEncoding class.
 */
void
GPRGuardEncodingTest::testCreation() {

    const string rf1 = "rf1";

    GPRGuardEncoding* enc1 = new GPRGuardEncoding(rf1, 0, false, 0, *gField_);
    TS_ASSERT(gField_->hasGPRGuardEncoding(rf1, 0, false));
    TS_ASSERT_THROWS(
	new GPRGuardEncoding(rf1, 0, false, 1, *gField_), ObjectAlreadyExists);
    TS_ASSERT_THROWS(
	new GPRGuardEncoding(rf1, 0, true, 0, *gField_), ObjectAlreadyExists);
    
    TS_ASSERT(enc1->registerFile() == rf1);
    TS_ASSERT(enc1->registerIndex() == 0);
    TS_ASSERT(!enc1->isGuardInverted());
    TS_ASSERT(enc1->encoding() == 0);
    TS_ASSERT(enc1->parent() == gField_);
}

#endif
