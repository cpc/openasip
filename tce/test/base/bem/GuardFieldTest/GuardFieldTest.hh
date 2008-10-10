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
 * @file GuardFieldTest.hh
 *
 * A test suite for GuardField.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
