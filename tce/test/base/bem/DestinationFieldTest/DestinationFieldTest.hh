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
