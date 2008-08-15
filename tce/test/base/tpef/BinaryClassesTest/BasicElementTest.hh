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
 * @file BasicElementTest.hh
 *
 * Tests for BasicElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
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
