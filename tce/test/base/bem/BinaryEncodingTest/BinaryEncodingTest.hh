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
 * @file BinaryEncodingTest.hh
 *
 * A test suite for BinaryEncoding.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef BinaryEncodingTest_HH
#define BinaryEncodingTest_HH

#include <string>

#include <TestSuite.h>
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "SocketCodeTable.hh"
#include "NullImmediateControlField.hh"

using std::string;

/**
 * Test suite for testing BinaryEncoding class.
 */
class BinaryEncodingTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testMoveSlots();
    void testSocketCodeTables();
    void testImmediateControlField();
    void testChildFields();

private:
    BinaryEncoding* bem_;
};


/**
 * Called before each test.
 */
void
BinaryEncodingTest::setUp() {
    bem_ = new BinaryEncoding();
}


/**
 * Called after each test.
 */
void
BinaryEncodingTest::tearDown() {
    delete bem_;
}


/**
 * Tests adding and removing move slots.
 */
void
BinaryEncodingTest::testMoveSlots() {
    
   const string bus1 = "b1";
   const string bus2 = "b2";

    TS_ASSERT(bem_->moveSlotCount() == 0);
    TS_ASSERT(bem_->childFieldCount() == 0);
    TS_ASSERT(bem_->bitPosition() == 0);
    TS_ASSERT(bem_->width() == 0);

    MoveSlot* slot1 = new MoveSlot(bus1, *bem_);
    MoveSlot* slot2 = new MoveSlot(bus2, *bem_);

    TS_ASSERT(bem_->moveSlotCount() == 2);
    TS_ASSERT(bem_->childFieldCount() == 2);
    TS_ASSERT(bem_->hasMoveSlot(bus1));
    TS_ASSERT(bem_->hasMoveSlot(bus2));
    TS_ASSERT(!bem_->hasMoveSlot("foobar"));
    TS_ASSERT(&bem_->moveSlot(bus1) == slot1);
    TS_ASSERT(&bem_->moveSlot(bus2) == slot2);
    
    TS_ASSERT(bem_->width() == 0);
    delete slot1;
    TS_ASSERT(bem_->moveSlotCount() == 1);
    
    TS_ASSERT_THROWS(bem_->moveSlot(1), OutOfRange);
    TS_ASSERT_THROWS(bem_->moveSlot(-1), OutOfRange);
    TS_ASSERT_THROWS_NOTHING(bem_->moveSlot(0));
}


/**
 * Tests adding and removing socket code tables.
 */
void
BinaryEncodingTest::testSocketCodeTables() {

    const string table1 = "table1";
    const string table2 = "table2";

    TS_ASSERT(bem_->socketCodeTableCount() == 0);
    SocketCodeTable* t1 = new SocketCodeTable(table1, *bem_);
    SocketCodeTable* t2 = new SocketCodeTable(table2, *bem_);
    TS_ASSERT(bem_->socketCodeTableCount() == 2);
    TS_ASSERT(&bem_->socketCodeTable(0) == t1);
    TS_ASSERT(&bem_->socketCodeTable(1) == t2);
    TS_ASSERT_THROWS(bem_->socketCodeTable(2), OutOfRange);
    TS_ASSERT_THROWS(bem_->socketCodeTable(-1), OutOfRange);
    TS_ASSERT(bem_->childFieldCount() == 0);
}


/**
 * Tests adding and removing immediate control field.
 */
void
BinaryEncodingTest::testImmediateControlField() {
    
    TS_ASSERT(!bem_->hasImmediateControlField());
    TS_ASSERT(
	&bem_->immediateControlField() == 
	&NullImmediateControlField::instance());
    ImmediateControlField* immField = new ImmediateControlField(*bem_);
    TS_ASSERT(bem_->hasImmediateControlField());
    TS_ASSERT(&bem_->immediateControlField() == immField);
    TS_ASSERT(bem_->childFieldCount() == 1);
    TS_ASSERT(&bem_->childField(0) == immField);
    TS_ASSERT(bem_->width() == 0);
}


/**
 * Tests switching the order of move slots and immediate control field.
 */
void
BinaryEncodingTest::testChildFields() {

    const string bus1 = "b1";
    const string bus2 = "b2";
    const string bus3 = "b3";

    MoveSlot* slot1 = new MoveSlot(bus1, *bem_);
    MoveSlot* slot2 = new MoveSlot(bus2, *bem_);
    MoveSlot* slot3 = new MoveSlot(bus3, *bem_);
    ImmediateControlField* icField = new ImmediateControlField(*bem_);
    
    TS_ASSERT(bem_->childFieldCount() == 4);
    TS_ASSERT(&bem_->childField(0) == slot1);
    TS_ASSERT(&bem_->childField(1) == slot2);
    TS_ASSERT(&bem_->childField(2) == slot3);
    TS_ASSERT(&bem_->childField(3) == icField);

    slot1->setRelativePosition(2);
    TS_ASSERT(bem_->childFieldCount() == 4);
    TS_ASSERT(&bem_->childField(0) == slot2);
    TS_ASSERT(&bem_->childField(1) == slot3);
    TS_ASSERT(&bem_->childField(2) == slot1);
    TS_ASSERT(&bem_->childField(3) == icField);

    icField->setRelativePosition(1);
    TS_ASSERT(bem_->childFieldCount() == 4);
    TS_ASSERT(&bem_->childField(0) == slot2);
    TS_ASSERT(&bem_->childField(1) == icField);
    TS_ASSERT(&bem_->childField(2) == slot3);
    TS_ASSERT(&bem_->childField(3) == slot1);
}

#endif
