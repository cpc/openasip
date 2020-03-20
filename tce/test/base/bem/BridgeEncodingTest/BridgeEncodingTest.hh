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
