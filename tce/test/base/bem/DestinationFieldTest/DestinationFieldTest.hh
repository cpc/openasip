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
 * @file DestinationFieldTest.hh
 *
 * A test suite for DestinationField.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
