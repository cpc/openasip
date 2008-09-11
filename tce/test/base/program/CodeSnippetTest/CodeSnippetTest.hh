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
