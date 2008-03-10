/**
 * @file BinaryTest.hh
 *
 * A test suite for Binary class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
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
