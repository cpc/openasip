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
 * @file BridgeEncodingTest.hh
 *
 * A test suite for BridgeEncoding.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef BridgeEncodingTest_HH
#define BridgeEncodingTest_HH

#include <string>

#include <TestSuite.h>
#include "BinaryEncoding.hh"
#include "BridgeEncoding.hh"
#include "MoveSlot.hh"
#include "SourceField.hh"

using std::string;

/**
 * Test suite for testing BridgeEncoding class.
 */
class BridgeEncodingTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();

private:
    BinaryEncoding* bem_;
    SourceField* sField_;
};


/**
 * Called before each test.
 */
void
BridgeEncodingTest::setUp() {
    bem_ = new BinaryEncoding();
    MoveSlot* slot = new MoveSlot("b1", *bem_);
    sField_ = new SourceField(BinaryEncoding::RIGHT, *slot);
}


/**
 * Called after each test.
 */
void
BridgeEncodingTest::tearDown() {
    delete bem_;
}


/**
 * Tests the constructor of BridgeEncoding class.
 */
void
BridgeEncodingTest::testCreation() {

    const string br1 = "br1";

    BridgeEncoding* enc1 = new BridgeEncoding(br1, 2, 0, *sField_);
    TS_ASSERT(enc1->parent() == sField_);
    TS_ASSERT(enc1->encoding() == 2);
    TS_ASSERT(enc1->bridgeName() == br1);
    TS_ASSERT(enc1->width() == 2);
}

#endif
