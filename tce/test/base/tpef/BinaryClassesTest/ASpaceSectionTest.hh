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
 * @file ASpaceSectionTest.hh 
 *
 * A test suite for ASpaceSection and ASpaceElements.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 */

#ifndef ASpaceSectionTest_HH
#define ASpaceSectionTest_HH

#include <TestSuite.h>
#include "ASpaceSection.hh"

using namespace TPEF;

/**
 * Test class for string section.
 */
class ASpaceSectionTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    
    void testSection();

private:
    static ASpaceSection *testSection_;
};

/**
 * Called before each test.
 * 
 * Creates newly initialized section for test to use.
 */
void
ASpaceSectionTest::setUp() {
    if (testSection_ != NULL) {
	delete testSection_;
	testSection_ = NULL;
    }

    testSection_ = dynamic_cast<ASpaceSection*>(
	Section::createSection(Section::ST_ADDRSP));
    TS_ASSERT_DIFFERS(testSection_, static_cast<Section*>(NULL));
}


/**
 * Called after each test.
 */
void
ASpaceSectionTest::tearDown() {
    delete testSection_;
    testSection_ = NULL;
}


/**
 * Tests simple methods of section.
 */
void
ASpaceSectionTest::testSection() {
    ASpaceElement *undefElement1 = new ASpaceElement();
    ASpaceElement *undefElement2 = new ASpaceElement();
    ASpaceElement *element1 = new ASpaceElement();;
    ASpaceElement *element2 = new ASpaceElement();;

    undefElement1->setMAU(0);
    undefElement1->setAlign(0);
    undefElement1->setWordSize(0);

    undefElement2->setMAU(0);
    undefElement2->setAlign(0);
    undefElement2->setWordSize(0);

    element1->setMAU(3);
    element1->setAlign(4);
    element1->setWordSize(2);

    element2->setMAU(10);
    element2->setAlign(2);
    element2->setWordSize(5);
    
    testSection_->addElement(element1);
    testSection_->addElement(undefElement1);
    testSection_->addElement(element2);
    testSection_->addElement(undefElement2);
    
    testSection_->setUndefinedASpace(undefElement1);

    TS_ASSERT_EQUALS(testSection_->isUndefined(undefElement1), true);
    TS_ASSERT_EQUALS(testSection_->isUndefined(undefElement2), false);
    TS_ASSERT_EQUALS(testSection_->isUndefined(element1), false);
    TS_ASSERT_EQUALS(testSection_->isUndefined(element2), false);

    testSection_->setUndefinedASpace(undefElement2);

    TS_ASSERT_EQUALS(testSection_->isUndefined(undefElement1), false);
    TS_ASSERT_EQUALS(testSection_->isUndefined(undefElement2), true);
    TS_ASSERT_EQUALS(testSection_->isUndefined(element1), false);
    TS_ASSERT_EQUALS(testSection_->isUndefined(element2), false);
}

ASpaceSection* ASpaceSectionTest::testSection_ = NULL;

#endif
