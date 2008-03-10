/**
 * @file GuardFieldTest.hh
 *
 * A test suite for GuardField.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef GuardFieldTest_HH
#define GuardFieldTest_HH

#include <string>

#include <TestSuite.h>
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "GuardField.hh"
#include "NullGPRGuardEncoding.hh"
#include "NullFUGuardEncoding.hh"

using std::string;

/**
 * Test suite for testing GuardField class.
 */
class GuardFieldTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();
    void testGuardEncodings();

private:
    BinaryEncoding* bem_;
    MoveSlot* slot_;
};


/**
 * Called before each test.
 */
void
GuardFieldTest::setUp() {
    bem_ = new BinaryEncoding();
    slot_ = new MoveSlot("bus", *bem_);
}


/**
 * Called after each test.
 */
void
GuardFieldTest::tearDown() {
    delete bem_;
}


/**
 * Tests the constructor of GuardField class.
 */
void
GuardFieldTest::testCreation() {
    GuardField* field = new GuardField(*slot_);
    TS_ASSERT(slot_->hasGuardField());
    TS_ASSERT(field->parent() == slot_);
    TS_ASSERT_THROWS(new GuardField(*slot_), ObjectAlreadyExists);
}


/**
 * Tests adding and removing guard encodings.
 */
void
GuardFieldTest::testGuardEncodings() {
    
    const string rf1 = "rf1";
    const string fu1 = "fu1";
    const string port1 = "p1";

    GuardField* field = new GuardField(*slot_);
    TS_ASSERT(field->gprGuardEncodingCount() == 0);
    TS_ASSERT(field->fuGuardEncodingCount() == 0);

    GPRGuardEncoding* gge = new GPRGuardEncoding(rf1, 0, false, 0, *field);
    TS_ASSERT(field->gprGuardEncodingCount() == 1);
    TS_ASSERT(field->childFieldCount() == 0);
    TS_ASSERT(field->width() == 1);
    TS_ASSERT(&field->gprGuardEncoding(0) == gge);
    TS_ASSERT_THROWS(field->gprGuardEncoding(-1), OutOfRange);
    TS_ASSERT_THROWS(field->gprGuardEncoding(1), OutOfRange);
    TS_ASSERT(field->hasGPRGuardEncoding(rf1, 0, false));
    TS_ASSERT(!field->hasGPRGuardEncoding(rf1, 0, true));
    TS_ASSERT(&field->gprGuardEncoding(rf1, 0, false) == gge);
    TS_ASSERT(
	&field->gprGuardEncoding(rf1, 1, false) == 
	&NullGPRGuardEncoding::instance());
    delete gge;
    
    FUGuardEncoding* fge = new FUGuardEncoding(fu1, port1, false, 0, *field);
    TS_ASSERT(field->fuGuardEncodingCount() == 1);
    TS_ASSERT(field->childFieldCount() == 0);
    TS_ASSERT(field->width() == 1);
    TS_ASSERT(&field->fuGuardEncoding(0) == fge);
    TS_ASSERT_THROWS(field->fuGuardEncoding(-1), OutOfRange);
    TS_ASSERT_THROWS(field->fuGuardEncoding(1), OutOfRange);
    TS_ASSERT(field->hasFUGuardEncoding(fu1, port1, false));
    TS_ASSERT(!field->hasFUGuardEncoding(fu1, port1, true));
    TS_ASSERT(&field->fuGuardEncoding(fu1, port1, false) == fge);
    TS_ASSERT(
	&field->fuGuardEncoding(fu1, port1, true) == 
	&NullFUGuardEncoding::instance());
    
}

#endif
