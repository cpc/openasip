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
 * @file SectionTest.hh
 *
 * A test suite for Section class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 */

#ifndef TTA_SECTION_TEST_HH
#define TTA_SECTION_TEST_HH

#include <list>
#include <TestSuite.h>

#include "Section.hh"
#include "BasicElement.hh"

using namespace TPEF;

///////////////////////////////////////////////////////////////////////////////
// SectionTest declaration
///////////////////////////////////////////////////////////////////////////////

/**
 * Tests for Section class.
 */
class SectionTest : public CxxTest::TestSuite {
public:
    void testCreateNullSection();
    void testAddElements();
    void testFlags();

    void setUp();
    void tearDown();
private:
    /// Number of elements that is added to Section.
    static const int ELEMENT_COUNT;
};

///////////////////////////////////////////////////////////////////////////////
// SectionTest implementation
///////////////////////////////////////////////////////////////////////////////

const int SectionTest::ELEMENT_COUNT = 5;

/**
 * Tests that creation of ST_NULL section is successful and if it's not then
 * no section is created.
 */
void 
SectionTest::testCreateNullSection() {	

    Section* section = NULL;
    try {
	section = Section::createSection(Section::ST_NULL);	
    } catch (InstanceNotFound e) {
	TS_ASSERT_EQUALS(section, static_cast<Section*>(NULL));
	return;
    }

    TS_ASSERT(section != NULL);
    delete section;
    section = NULL;
}

/**
 * Tests that adding elements to section works correctly.
 */
void 
SectionTest::testAddElements() {

    Section* section = Section::createSection(Section::ST_CODE);
    // comparing pointers should not be done with TS_ASSERT_DIFFERS macro
    TS_ASSERT(section != NULL);	
    
    // adding elements works
    for (int i = 0;i < ELEMENT_COUNT;i++) {
	SectionElement* element = new BasicElement();
	section->addElement(element);
	TS_ASSERT_EQUALS(static_cast<int>(section->elementCount()), i + 1);
    }
    
    TS_ASSERT_EQUALS(static_cast<int>(section->elementCount()), ELEMENT_COUNT);		
    delete section;
    section = NULL;
}

/**
 * Tests flag handling methods.
 */
void 
SectionTest::testFlags() {

    Section* section = Section::createSection(Section::ST_DATA);
    TS_ASSERT(section != NULL);	
    
    section->setFlagNoBits();
    TS_ASSERT(!section->vLen());
    TS_ASSERT(section->noBits());
    
    section->unsetFlagNoBits();
    TS_ASSERT(!section->vLen());
    TS_ASSERT(!section->noBits());
    
    delete section;
    section = NULL;

    section = Section::createSection(Section::ST_CODE);
    TS_ASSERT(section != NULL);	

    section->setFlagNoBits();
    TS_ASSERT(section->vLen());
    TS_ASSERT(section->noBits());
    
    section->unsetFlagNoBits();
    TS_ASSERT(section->vLen());
    TS_ASSERT(!section->noBits());

    delete section;
    section = NULL;
}

/**
 * Initialization code of each test
 */
void 
SectionTest::setUp() {
}

/**
 * Cleanup code of each test.
 */
void 
SectionTest::tearDown() {
}

#endif
