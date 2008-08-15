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
 * @file RelocSectionTest.hh
 *
 * A test suite for RelocSection.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @note reviewed 7 November 2003 by pj, ml, am, ml
 */

#ifndef TTA_RELOC_SECTION_TEST_HH
#define TTA_RELOC_SECTION_TEST_HH

#include <TestSuite.h>
#include "Section.hh"
#include "RelocSection.hh"
#include "RelocElement.hh"
#include "ASpaceElement.hh"
#include "NoTypeSymElement.hh"
#include "ImmediateElement.hh"
#include "MoveElement.hh"
#include "Exception.hh"

using namespace TPEF;

/**
 * Test class for relocation section.
 */
class RelocSectionTest : public CxxTest::TestSuite {
public:
    void testChunkMethod();
    void testElementsMethods();
    void setUp();
    void tearDown();
private:
    static const int LOCATION;
    static const int SIZE_OF_DESTINATION_VALUE;
    static RelocSection *testSection_;
    static RelocElement *elem_;
    static MoveElement  *dest_;
    static ImmediateElement *src_;
    static NoTypeSymElement *sym_;
    static ASpaceElement *aSpace_;
};

const int RelocSectionTest::LOCATION = 2000;
const int RelocSectionTest::SIZE_OF_DESTINATION_VALUE = 4;

RelocSection     *RelocSectionTest::testSection_ = NULL;
RelocElement     *RelocSectionTest::elem_        = NULL;
MoveElement      *RelocSectionTest::dest_        = NULL;
ImmediateElement *RelocSectionTest::src_         = NULL;
NoTypeSymElement *RelocSectionTest::sym_         = NULL;
ASpaceElement    *RelocSectionTest::aSpace_      = NULL;

/**
 * Initializes data for tests.
 */
inline void
RelocSectionTest::setUp() {
    testSection_ = dynamic_cast<RelocSection*>(
	Section::createSection(Section::ST_RELOC));

    elem_ = new RelocElement();
    dest_ = new MoveElement();
    src_ = new ImmediateElement();
    sym_ = new NoTypeSymElement();
    aSpace_ = new ASpaceElement();

    elem_->setType(RelocElement::RT_NOREL);
    elem_->setLocation(src_);
    elem_->setDestination(dest_);
    elem_->setSize(SIZE_OF_DESTINATION_VALUE);
    elem_->setBitOffset(0);
    elem_->setSymbol(sym_);
    elem_->setASpace(aSpace_);
    elem_->setChunked(false);
    
    testSection_->addElement(elem_);
}

/**
 * Cleans up after each test.
 */
inline void
RelocSectionTest::tearDown() {
    delete testSection_;
    testSection_ = NULL;
    delete dest_;
    dest_ = NULL;
    delete src_;
    src_ = NULL;
    delete sym_;
    sym_ = NULL;
    delete aSpace_;
    aSpace_ = NULL;
}


/**
 * Tests that chunk method from relocation section throws NotChunkable 
 * exception.
 */
inline void
RelocSectionTest::testChunkMethod() {
    Section* relocSection = Section::createSection(Section::ST_RELOC);
    Chunk* testChunk = NULL;
    TS_ASSERT_THROWS(testChunk = relocSection->chunk(LOCATION),
                     NotChunkable);
    
    delete relocSection;
    relocSection = NULL;
}

inline void 
RelocSectionTest::testElementsMethods() {
    TS_ASSERT_EQUALS(static_cast<int>(testSection_->elementCount()),1);
    
    RelocElement *reloc = dynamic_cast<RelocElement*> 
	(testSection_->element(0));
    
    TS_ASSERT_EQUALS(reloc, elem_);
    TS_ASSERT_EQUALS(reloc->type(), RelocElement::RT_NOREL);
    TS_ASSERT_EQUALS(reloc->location(), src_);
    TS_ASSERT_EQUALS(reloc->destination(), dest_);
    TS_ASSERT_EQUALS(reloc->size(), 4);
    TS_ASSERT_EQUALS(reloc->bitOffset(), 0);
    TS_ASSERT_EQUALS(reloc->symbol(), sym_);
    TS_ASSERT_EQUALS(reloc->aSpace(), aSpace_);
    TS_ASSERT_EQUALS(reloc->chunked(), false);
}

#endif
