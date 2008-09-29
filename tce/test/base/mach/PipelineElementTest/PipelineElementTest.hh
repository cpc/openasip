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
 * @file PipelineElementTest.hh 
 * A test suite for PipelineElement.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef PipelineElementTest_HH
#define PipelineElementTest_HH

#include <TestSuite.h>
#include "FunctionUnit.hh"
#include "PipelineElement.hh"

using std::string;
using namespace TTAMachine;

class PipelineElementTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testSetName();

private:
};


/**
 * Called before each test.
 */
void
PipelineElementTest::setUp() {
}


/**
 * Called after each test.
 */
void
PipelineElementTest::tearDown() {
}


/**
 * Tests setting the name of the pipeline element.
 */
void
PipelineElementTest::testSetName() {

    FunctionUnit* fu = new FunctionUnit("fu");
    PipelineElement* res1 = new PipelineElement("res1", *fu);
    PipelineElement* res2 = new PipelineElement("res2", *fu);

    TS_ASSERT_THROWS_NOTHING(res1->setName("foo"));
    TS_ASSERT(res1->name() == "foo");
    TS_ASSERT_THROWS(res2->setName("foo"), ComponentAlreadyExists);
    TS_ASSERT(res2->name() == "res2");

    delete res1;
    delete res2;
    delete fu;
}
    
#endif
