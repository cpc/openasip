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
