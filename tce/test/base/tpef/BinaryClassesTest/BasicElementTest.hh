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
 * @file BasicElementTest.hh
 *
 * Tests for BasicElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 */

#ifndef TTA_BASIC_ELEMENT_TEST_HH
#define TTA_BASIC_ELEMENT_TEST_HH

#include <TestSuite.h>
#include <vector>

#include "BasicElement.hh"

using std::vector;
using namespace TPEF;

/**
 * Tests for BasicElement class.
 */
class BasicElementTest : public CxxTest::TestSuite {
public:
    void testSetAndGet();

    void setUp();
    void tearDown();

private:
    static const Word DATA_LENGTH;

    /// Test element.
    static BasicElement element_;
    /// Test data.
    static vector<Byte> bytes_;
};

BasicElement BasicElementTest::element_;
vector<Byte> BasicElementTest::bytes_;
const Word BasicElementTest::DATA_LENGTH = 1000;

/**
 * Tests setBytes and getBytes methods.
 */ 
void 
BasicElementTest::testSetAndGet() {
    for (Word i = 0; i < bytes_.size(); i++) {
	element_.addByte(bytes_[i]);
    }
    
    // vector where to collect data from element
    vector<Byte> temp;
    for(Word i = 0; i < element_.length(); i++) {
	temp.push_back(element_.byte(i));
    }
    
    TS_ASSERT(temp == bytes_);
}

/**
 * Creates test data.
 */
void 
BasicElementTest::setUp() {
    for (Word i = 0;i < DATA_LENGTH;i++) {
	bytes_.push_back(static_cast<Byte>(i%256));
    }
}

/**
 * Test suite's cleanup method.
 */
void 
BasicElementTest::tearDown() {
}

#endif
