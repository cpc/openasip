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
