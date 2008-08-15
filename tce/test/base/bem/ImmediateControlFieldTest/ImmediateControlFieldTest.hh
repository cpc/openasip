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
 * @file ImmediateControlFieldTest.hh
 *
 * A test suite for ImmediateControlField.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef ImmediateControlFieldTest_HH
#define ImmediateControlFieldTest_HH

#include <string>

#include <TestSuite.h>
#include "BinaryEncoding.hh"
#include "ImmediateControlField.hh"

using std::string;

/**
 * Test suite for testing ImmediateControlField class.
 */
class ImmediateControlFieldTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();
    void testTemplateEncodings();

private:
    BinaryEncoding* bem_;
};


/**
 * Called before each test.
 */
void
ImmediateControlFieldTest::setUp() {
    bem_ = new BinaryEncoding();
}


/**
 * Called after each test.
 */
void
ImmediateControlFieldTest::tearDown() {
    delete bem_;
}


/**
 * Tests the constructor of ImmediateControlField class.
 */
void
ImmediateControlFieldTest::testCreation() {
    ImmediateControlField* field = new ImmediateControlField(*bem_);
    TS_ASSERT(field->parent() == bem_);
    TS_ASSERT(bem_->hasImmediateControlField());
    TS_ASSERT_THROWS(new ImmediateControlField(*bem_), ObjectAlreadyExists);   
    TS_ASSERT(field->width() == 0);
    TS_ASSERT(field->childFieldCount() == 0);
}


/**
 * Tests assigning encoding for instruction templates.
 */
void
ImmediateControlFieldTest::testTemplateEncodings() {
    
    const string temp1 = "t1";
    const string temp2 = "t2";
    const string temp3 = "t3";

    ImmediateControlField* field = new ImmediateControlField(*bem_);
    TS_ASSERT(field->templateCount() == 0);

    field->addTemplateEncoding(temp1, 0);
    field->addTemplateEncoding(temp2, 1);
    
    TS_ASSERT(field->childFieldCount() == 0);
    TS_ASSERT(field->width() == 1);
    TS_ASSERT(field->hasTemplateEncoding(temp1));
    TS_ASSERT(field->hasTemplateEncoding(temp2));
    TS_ASSERT(!field->hasTemplateEncoding("foobar"));
    
    TS_ASSERT_THROWS(
	field->addTemplateEncoding(temp1, 1), ObjectAlreadyExists);

    field->addTemplateEncoding(temp3, 2);
    TS_ASSERT(field->width() == 2);
    TS_ASSERT(field->hasTemplateEncoding(temp3));
    field->removeTemplateEncoding(temp3);
    TS_ASSERT(!field->hasTemplateEncoding(temp3));

    TS_ASSERT(field->templateEncoding(temp1) == 0);
    TS_ASSERT(field->templateEncoding(temp2) == 1);

    field->addTemplateEncoding(temp1, 5);
    TS_ASSERT(field->templateEncoding(temp1) == 5);

    TS_ASSERT_THROWS(field->instructionTemplate(-1), OutOfRange);
    TS_ASSERT_THROWS(field->instructionTemplate(2), OutOfRange);
    TS_ASSERT_THROWS_NOTHING(field->instructionTemplate(0));

    field->setExtraBits(2);
    TS_ASSERT(field->width() == 5);
}
   
#endif
