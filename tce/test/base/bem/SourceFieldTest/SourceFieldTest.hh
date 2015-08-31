/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file SourceFieldTest.hh
 *
 * A test suite for SourceField.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef SourceFieldTest_HH
#define SourceFieldTest_HH

#include <string>

#include <TestSuite.h>
#include "BinaryEncoding.hh"
#include "SourceField.hh"
#include "MoveSlot.hh"
#include "SocketEncoding.hh"
#include "BridgeEncoding.hh"

using std::string;

/**
 * Test suite for testing SourceField class.
 */
class SourceFieldTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();
    void testSocketEncodings();
    void testBridgeEncodings();

private:
    BinaryEncoding* bem_;
    MoveSlot* slot_;
};


/**
 * Called before each test.
 */
void
SourceFieldTest::setUp() {
    bem_ = new BinaryEncoding();
    slot_ = new MoveSlot("b1", *bem_);
}


/**
 * Called after each test.
 */
void
SourceFieldTest::tearDown() {
    delete bem_;
}


/**
 * Tests the constructor of SourceField class.
 */
void
SourceFieldTest::testCreation() {

    SourceField* field = new SourceField(BinaryEncoding::LEFT, *slot_);
    TS_ASSERT(field->parent() == slot_);
    TS_ASSERT(slot_->hasSourceField());
    TS_ASSERT_THROWS(
        new SourceField(BinaryEncoding::LEFT, *slot_),
	ObjectAlreadyExists);

    TS_ASSERT(field->width() == 0);
    TS_ASSERT(field->childFieldCount() == 0);
    TS_ASSERT(field->relativePosition() == 0);
    TS_ASSERT(field->bitPosition() == 0);
    TS_ASSERT(field->componentIDPosition() == BinaryEncoding::LEFT);

    MoveSlot* slot2 = new MoveSlot("b2", *bem_);
    TS_ASSERT_THROWS(new SourceField(BinaryEncoding::RIGHT, *slot2),
	IllegalParameters);
}


/**
 * Tests adding and removing socket encodings.
 */
void
SourceFieldTest::testSocketEncodings() {
    
    const string s1 = "s1";
    const string s2 = "s2";

    SourceField* field = new SourceField(BinaryEncoding::LEFT, *slot_);
    SocketEncoding* enc1 = new SocketEncoding(s1, 0, 1, *field);
    TS_ASSERT(field->socketEncodingCount() == 1);
    TS_ASSERT(field->hasSocketEncoding(s1));
    TS_ASSERT(&field->socketEncoding(s1) == enc1);
    TS_ASSERT(&field->socketEncoding(0) == enc1);
    TS_ASSERT_THROWS(field->socketEncoding(-1), OutOfRange);
    TS_ASSERT_THROWS(field->socketEncoding(1), OutOfRange);
    
    new SocketEncoding(s2, 1, 0, *field);
    TS_ASSERT(field->socketEncodingCount() == 2);
    delete enc1;
    TS_ASSERT(field->socketEncodingCount() == 1);
    TS_ASSERT(field->width() == 1);
    TS_ASSERT(field->childFieldCount() == 0);
}


/**
 * Tests adding and removing bridge encodings.
 */
void
SourceFieldTest::testBridgeEncodings() {
    
    const string br1 = "br1";
    const string br2 = "br2";
    const string br3 = "br3";
    
    SourceField* field = new SourceField(BinaryEncoding::LEFT, *slot_);
    BridgeEncoding* enc = new BridgeEncoding(br1, 0, 2, *field);
    TS_ASSERT(field->bridgeEncodingCount() == 1);
    TS_ASSERT(&field->bridgeEncoding(0) == enc);
    
    TS_ASSERT_THROWS(new SocketEncoding("s1", 0, 3, *field), 
		     ObjectAlreadyExists);
    TS_ASSERT_THROWS_NOTHING(new SocketEncoding("s1", 1, 1, *field));
    TS_ASSERT_THROWS(new BridgeEncoding(br2, 1, 2, *field), 
		     ObjectAlreadyExists);
    TS_ASSERT_THROWS(new BridgeEncoding(br1, 2, 0, *field), 
		     ObjectAlreadyExists);
    
    new BridgeEncoding(br2, 2, 0, *field);
    TS_ASSERT(field->bridgeEncodingCount() == 2);
    TS_ASSERT(field->width() == 2);
    TS_ASSERT_THROWS(new BridgeEncoding(br3, 6, 0, *field), 
		     ObjectAlreadyExists);

    delete enc;
    TS_ASSERT(field->bridgeEncodingCount() == 1);
    TS_ASSERT(field->childFieldCount() == 0);
}
    

#endif
