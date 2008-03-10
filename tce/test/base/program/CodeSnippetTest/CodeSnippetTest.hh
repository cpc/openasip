/** 
 * @file CodeSnippetTest.hh
 * 
 * A test suite for CodeSnippet.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
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
