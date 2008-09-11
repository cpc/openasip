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
 * @file MoveSlotTest.hh
 *
 * A test suite for MoveSlot.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
