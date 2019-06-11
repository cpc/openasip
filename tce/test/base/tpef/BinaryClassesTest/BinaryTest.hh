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
 * @file BinaryTest.hh
 *
 * A test suite for Binary class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 */

#ifndef TTA_BINARY_TEST_HH
#define TTA_BINARY_TEST_HH

#include <TestSuite.h>

#include "Binary.hh"
#include "Section.hh"

using namespace TPEF;

///////////////////////////////////////////////////////////////////////////////
// BinaryTest declaration
///////////////////////////////////////////////////////////////////////////////

/**
 * Tests for Binary class.
 */
class BinaryTest : public CxxTest::TestSuite {
public:
    void testAddAndQuerySection();

    void setUp();
    void tearDown();
};

///////////////////////////////////////////////////////////////////////////////
// BinaryTest implementation
///////////////////////////////////////////////////////////////////////////////

/**
 * Tests add and query methods of Binary class.
 */
void 
BinaryTest::testAddAndQuerySection() {
    
    Section* reloc1 = Section::createSection(Section::ST_RELOC);
    Section* reloc2 = Section::createSection(Section::ST_RELOC);
    Section* reloc3 = Section::createSection(Section::ST_RELOC);
    
    Section* data1 = Section::createSection(Section::ST_DATA);
    
    TS_ASSERT_DIFFERS(reloc1, static_cast<SafePointable*>(NULL));
    TS_ASSERT_DIFFERS(reloc2, static_cast<SafePointable*>(NULL));
    TS_ASSERT_DIFFERS(reloc3, static_cast<SafePointable*>(NULL));
    
    TS_ASSERT_DIFFERS(data1, static_cast<SafePointable*>(NULL));
    
    Binary testBin;
    
    TS_ASSERT_EQUALS(static_cast<int>(testBin.sectionCount()), 0);
    
    testBin.addSection(reloc1);
    testBin.addSection(reloc2);
    testBin.addSection(reloc3);
    
    testBin.addSection(data1);
    
    TS_ASSERT_EQUALS(static_cast<int>(testBin.sectionCount()), 4);
}

/**
 * Initialization method of test suite.
 */ 
void 
BinaryTest::setUp() {
}

/**
 * Cleanup method of test suite.
 */
void 
BinaryTest::tearDown() {
}

#endif
