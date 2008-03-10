/**
 * @file MoveSlotTest.hh
 *
 * A test suite for MoveSlot.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef MoveSlotTest_HH
#define MoveSlotTest_HH

#include <string>

#include <TestSuite.h>
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "NullGuardField.hh"
#include "NullSourceField.hh"
#include "NullDestinationField.hh"

using std::string;

/**
 * Test suite for testing MoveSlot class.
 */
class MoveSlotTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testNames();
    void testGuardField();
    void testSourceField();
    void testDestinationField();
    
private:
    BinaryEncoding* bem_;
};


/**
 * Called before each test.
 */
void
MoveSlotTest::setUp() {
    bem_ = new BinaryEncoding();
}


/**
 * Called after each test.
 */
void
MoveSlotTest::tearDown() {
    delete bem_;
}


/**
 * Tests setting the name of the move slot.
 */
void
MoveSlotTest::testNames() {

    const string bus1 = "bus1";
    const string bus2 = "bus2";
    const string foo = "foo";

    MoveSlot* slot1 = new MoveSlot(bus1, *bem_);
    new MoveSlot(bus2, *bem_);
    
    TS_ASSERT_THROWS(new MoveSlot(bus1, *bem_), ObjectAlreadyExists);
    TS_ASSERT_THROWS(slot1->setName(bus2), ObjectAlreadyExists);
    TS_ASSERT(slot1->name() == bus1);
    
    slot1->setName(foo);
    TS_ASSERT(slot1->name() == foo);
}


/**
 * Tests adding and removing guard field.
 */
void
MoveSlotTest::testGuardField() {
    
    MoveSlot* slot = new MoveSlot("b1", *bem_);
    TS_ASSERT(!slot->hasGuardField());
    TS_ASSERT(&slot->guardField() == &NullGuardField::instance());

    GuardField* gField = new GuardField(*slot);
    TS_ASSERT(slot->hasGuardField());
    TS_ASSERT(&slot->guardField() == gField);

    TS_ASSERT(slot->childFieldCount() == 1);
    TS_ASSERT(&slot->childField(0) == gField);

    TS_ASSERT(slot->width() == 0);

    delete gField;
    TS_ASSERT(!slot->hasGuardField());
}


/**
 * Tests adding and removing source field.
 */
void
MoveSlotTest::testSourceField() {

    MoveSlot* slot = new MoveSlot("b1", *bem_);
    TS_ASSERT(!slot->hasSourceField());
    TS_ASSERT(&slot->sourceField() == &NullSourceField::instance());

    SourceField* sField = new SourceField(BinaryEncoding::LEFT, *slot);
    TS_ASSERT(slot->hasSourceField());
    TS_ASSERT(&slot->sourceField() == sField);

    TS_ASSERT(slot->childFieldCount() == 1);
    TS_ASSERT(&slot->childField(0) == sField);

    TS_ASSERT_EQUALS(slot->width(), 0);

    delete sField;
    TS_ASSERT(!slot->hasSourceField());
}


/**
 * Tests adding and removing destination field.
 */
void
MoveSlotTest::testDestinationField() {

    MoveSlot* slot = new MoveSlot("b1", *bem_);
    TS_ASSERT(!slot->hasDestinationField());
    TS_ASSERT(&slot->destinationField() == &NullDestinationField::instance());

    DestinationField* dField = 
	new DestinationField(BinaryEncoding::LEFT, *slot);
    TS_ASSERT(slot->hasDestinationField());
    TS_ASSERT(&slot->destinationField() == dField);

    TS_ASSERT(slot->childFieldCount() == 1);
    TS_ASSERT(&slot->childField(0) == dField);

    TS_ASSERT(slot->width() == 0);

    delete dField;
    TS_ASSERT(!slot->hasDestinationField());
}

#endif
