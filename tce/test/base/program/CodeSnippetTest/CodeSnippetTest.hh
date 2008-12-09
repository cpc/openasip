/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file CodeSnippetTest.hh
 * 
 * A test suite for CodeSnippet.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef CODE_SNIPPET_TEST_HH
#define CODE_SNIPPET_TEST_HH

#include <TestSuite.h>
#include "CodeSnippet.hh"
#include "Procedure.hh"
#include "NullAddress.hh"
#include "NullAddressSpace.hh"

using namespace TTAMachine;
using namespace TTAProgram;

class CodeSnippetTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testCodeSnippet();
};


/**
 * Called before each test.
 */
void
CodeSnippetTest::setUp() {
}


/**
 * Called after each test.
 */
void
CodeSnippetTest::tearDown() {
}


/**
 * Tests that CodeSnippet works properly.
 */
void
CodeSnippetTest::testCodeSnippet() {

    Instruction* ins1 = new Instruction;
    Instruction* ins2 = new Instruction;
    Instruction* ins3 = new Instruction;

    CodeSnippet snippet;

    TS_ASSERT_EQUALS(snippet.instructionCount(), 0);

    TS_ASSERT_THROWS_NOTHING(snippet.add(ins1));
    TS_ASSERT_THROWS_NOTHING(snippet.add(ins2));
    TS_ASSERT_THROWS_NOTHING(snippet.add(ins3));

    TS_ASSERT_EQUALS(snippet.instructionCount(), 3);

    snippet.removeLastInstruction();

    TS_ASSERT_EQUALS(snippet.instructionCount(), 2);
    TS_ASSERT_EQUALS(&snippet.instructionAtIndex(0), ins1);
    TS_ASSERT_EQUALS(&snippet.instructionAtIndex(1), ins2);

    Procedure proc(
        "procedure", NullAddressSpace::instance());

    TS_ASSERT_THROWS(snippet.add(ins2), IllegalRegistration);
}

#endif
