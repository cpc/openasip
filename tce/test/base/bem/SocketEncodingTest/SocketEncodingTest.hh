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
 * @file SocketEncodingTest.hh
 *
 * A test suite for SocketEncoding.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef SocketEncodingTest_HH
#define SocketEncodingTest_HH

#include <string>

#include <TestSuite.h>
#include "BinaryEncoding.hh"
#include "SocketEncoding.hh"
#include "MoveSlot.hh"
#include "SourceField.hh"
#include "BridgeEncoding.hh"
#include "NullSocketCodeTable.hh"

using std::string;

/**
 * Test suite for testing SocketEncoding class.
 */
class SocketEncodingTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();
    void testSocketCodes();

private:
    BinaryEncoding* bem_;
    SourceField* sField_;
};


/**
 * Called before each test.
 */
void
SocketEncodingTest::setUp() {
    bem_ = new BinaryEncoding();
    MoveSlot* slot = new MoveSlot("b1", *bem_);
    sField_ = new SourceField(BinaryEncoding::RIGHT, *slot);
}


/**
 * Called after each test.
 */
void
SocketEncodingTest::tearDown() {
    delete bem_;
}


/**
 * Tests the constructor of SocketEncoding class.
 */
void
SocketEncodingTest::testCreation() {
    
    const string s1 = "s1";
    const string s2 = "s2";
    const string s3 = "s3";
    
    SocketEncoding* enc1 = new SocketEncoding(s1, 0, 1, *sField_);
    TS_ASSERT(enc1->parent() == sField_);
    TS_ASSERT(sField_->socketEncodingCount() == 1);
    TS_ASSERT(enc1->socketName() == s1);
    TS_ASSERT(enc1->encoding() == 0);

    SocketEncoding* enc2 = new SocketEncoding(s2, 1, 0, *sField_);
    TS_ASSERT(sField_->socketEncodingCount() == 2);
    TS_ASSERT_THROWS(
	new SocketEncoding(s1, 2, 0, *sField_), ObjectAlreadyExists);
    TS_ASSERT_THROWS(
	new SocketEncoding(s3, 1, 0, *sField_), ObjectAlreadyExists);

    delete enc1;
    delete enc2;

    new BridgeEncoding("br1", 2, 0, *sField_);
    TS_ASSERT_THROWS(
	new SocketEncoding(s3, 2, 0, *sField_), ObjectAlreadyExists);

    TS_ASSERT(sField_->socketEncodingCount() == 0);
}


/**
 * Tests setting and unsetting socket code table.
 */
void
SocketEncodingTest::testSocketCodes() {

    SocketEncoding* enc1 = new SocketEncoding("s1", 0, 0, *sField_);
    TS_ASSERT(!enc1->hasSocketCodes());
    TS_ASSERT(&enc1->socketCodes() == &NullSocketCodeTable::instance());
    
    TS_ASSERT(enc1->socketIDWidth() == 0);
    TS_ASSERT(enc1->extraBits() == 0);
    TS_ASSERT(enc1->width() == 0);

    SocketCodeTable* table = new SocketCodeTable("t1", *bem_);
    enc1->setSocketCodes(*table);
    TS_ASSERT(enc1->hasSocketCodes());
    TS_ASSERT(&enc1->socketCodes() == table);
    TS_ASSERT(enc1->width() == 0);
    
}

#endif
