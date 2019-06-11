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
