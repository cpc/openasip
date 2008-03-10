/**
 * @file DestinationFieldTest.hh
 *
 * A test suite for DestinationField.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef DestinationFieldTest_HH
#define DestinationFieldTest_HH

#include <string>

#include <TestSuite.h>
#include "BinaryEncoding.hh"
#include "DestinationField.hh"
#include "MoveSlot.hh"

using std::string;

/**
 * Test suite for testing DestinationField class.
 */
class DestinationFieldTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();
private:
    BinaryEncoding* bem_;
    MoveSlot* slot_;
};


/**
 * Called before each test.
 */
void
DestinationFieldTest::setUp() {
    bem_ = new BinaryEncoding();
    slot_ = new MoveSlot("b1", *bem_);
}


/**
 * Called after each test.
 */
void
DestinationFieldTest::tearDown() {
    delete bem_;
}


/**
 * Tests the constructor of DestinationField class.
 */
void
DestinationFieldTest::testCreation() {
    
    DestinationField* field = 
	new DestinationField(BinaryEncoding::LEFT, *slot_);
    TS_ASSERT(slot_->hasDestinationField());
    TS_ASSERT(field->parent() == slot_);
    TS_ASSERT_THROWS(
	new DestinationField(BinaryEncoding::LEFT, *slot_),
	ObjectAlreadyExists);
    TS_ASSERT_THROWS(
	new DestinationField(BinaryEncoding::RIGHT, *slot_), 
	IllegalParameters);
    delete field;
    TS_ASSERT(!slot_->hasDestinationField());
}
    
#endif
