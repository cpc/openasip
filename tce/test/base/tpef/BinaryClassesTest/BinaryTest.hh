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
