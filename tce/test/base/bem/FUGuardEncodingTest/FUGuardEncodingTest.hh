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
 * @file FUGuardEncodingTest.hh
 *
 * A test suite for FUGuardEncoding.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef FUGuardEncodingTest_HH
#define FUGuardEncodingTest_HH

#include <string>

#include <TestSuite.h>
#include "BinaryEncoding.hh"
#include "FUGuardEncoding.hh"
#include "MoveSlot.hh"
#include "GuardField.hh"

using std::string;

/**
 * Test suite for testing FUGuardEncoding class.
 */
class FUGuardEncodingTest : public CxxTest::TestSuite {
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
FUGuardEncodingTest::setUp() {
    bem_ = new BinaryEncoding();
    MoveSlot* slot = new MoveSlot("b1", *bem_);
    gField_ = new GuardField(*slot);
}


/**
 * Called after each test.
 */
void
FUGuardEncodingTest::tearDown() {
    delete bem_;
}


/**
 * Tests the constructor of FUGuardEncoding class.
 */
void
FUGuardEncodingTest::testCreation() {

    const string fu1 = "fu1";
    const string port1 = "port1";

    FUGuardEncoding* enc1 = 
	new FUGuardEncoding(fu1, port1, false, 0, *gField_);
    TS_ASSERT(gField_->hasFUGuardEncoding(fu1, port1, false));
    TS_ASSERT_THROWS(
	new FUGuardEncoding(fu1, port1, false, 1, *gField_), 
	ObjectAlreadyExists);
    TS_ASSERT_THROWS(
	new FUGuardEncoding(fu1, port1, true, 0, *gField_), 
	ObjectAlreadyExists);
    
    TS_ASSERT(enc1->functionUnit() == fu1);
    TS_ASSERT(enc1->port() == port1);
    TS_ASSERT(!enc1->isGuardInverted());
    TS_ASSERT(enc1->encoding() == 0);
    TS_ASSERT(enc1->parent() == gField_);
}

#endif
