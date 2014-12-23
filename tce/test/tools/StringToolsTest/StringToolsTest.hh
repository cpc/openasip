/*
    Copyright (c) 2002-2014 Tampere University of Technology.

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
 * @file StringToolsTest.hh
 *
 * A test suite for StringTools class.
 *
 * @author Pekka J‰‰skel‰inen 2004,2014 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_STRINGTOOLS_TEST_HH
#define TTA_STRINGTOOLS_TEST_HH

#include <string>
#include <vector>

#include <TestSuite.h>
#include "StringTools.hh"


using std::string;
using std::vector;


/**
 * Implements the tests needed to verify correct operation of StringTools
 * and the TCEString class.
 */
class StringToolsTest : public CxxTest::TestSuite {

public:
    void testTrim();
    void testStringToCharPtr();
    void testContainsChar();
    void testStringToUpper();
    void testStringToLower();
    void testChopString();
    void testSplitToRows();
    void testEndsWith();
    void replaceString();
};


/**
 * Test trim().
 */
void 
StringToolsTest::testTrim() {
    TS_ASSERT_EQUALS(StringTools::trim("    abc d  e f    "), "abc d  e f");
    TS_ASSERT_EQUALS(StringTools::trim(""), "");
    TS_ASSERT_EQUALS(StringTools::trim("       "), "");
    TS_ASSERT_EQUALS(StringTools::trim("werer       "), "werer");
    TS_ASSERT_EQUALS(StringTools::trim("\n\t\n\t        werer"), "werer");
}


/**
 * Test string2char().
 */
void
StringToolsTest::testStringToCharPtr() {
    string test = "foo";
    char* correct = new char[test.size() + 1];
    correct[0] = 'f';
    correct[1] = 'o';
    correct[2] = 'o';
    correct[3] = 0;
    char* testChar = StringTools::stringToCharPtr(test);
    TS_ASSERT_EQUALS(string(testChar), string(correct));
    delete[] correct;
    delete[] testChar;
}


/**
 * Test containsChar().
 */
void
StringToolsTest::testContainsChar() {
    string testString = "Jussi";
    TS_ASSERT_EQUALS(StringTools::containsChar(testString, 'J'), true);
    TS_ASSERT_EQUALS(StringTools::containsChar(testString, 'u'), true);
    TS_ASSERT_EQUALS(StringTools::containsChar(testString, 'j'), false);
    TS_ASSERT_EQUALS(StringTools::containsChar(testString, 'j', false), true);
}


/**
 * Test stringToUpper().
 */
void
StringToolsTest::testStringToUpper() {
    string testString1 = "diididaa";
    string testString2 = "DiiDiDaa";
    TS_ASSERT_EQUALS(StringTools::stringToUpper(testString1), "DIIDIDAA");
    TS_ASSERT_EQUALS(StringTools::stringToUpper(testString2), "DIIDIDAA");
}


/**
 * Test stringToLower().
 */
void 
StringToolsTest::testStringToLower() {
    
    string testString1 = "DIIDAA";
    string testString2 = "DiiDaa";
    
    TS_ASSERT_EQUALS(StringTools::stringToLower(testString1), "diidaa");
    TS_ASSERT_EQUALS(StringTools::stringToLower(testString2), "diidaa");
}


/**
 * Test chopString().
 */
void
StringToolsTest::testChopString() {
    string testString1 = "jussi,  pekka,  ari";
    string testString2 = "foo bar foo";
    string testString3 = "foobar    ";

    vector<TCEString> results = StringTools::chopString(testString1, ",");
    TS_ASSERT_EQUALS(static_cast<int>(results.size()), 3);
    TS_ASSERT_EQUALS(results[0], "jussi");
    TS_ASSERT_EQUALS(results[1], "pekka");
    TS_ASSERT_EQUALS(results[2], "ari");
    results.clear();

    results = StringTools::chopString(testString2, " ");
    TS_ASSERT_EQUALS(static_cast<int>(results.size()), 3);
    TS_ASSERT_EQUALS(results[0], "foo");
    TS_ASSERT_EQUALS(results[1], "bar");
    TS_ASSERT_EQUALS(results[2], "foo");
    results.clear();

    results = StringTools::chopString(testString3, " ");
    TS_ASSERT_EQUALS(static_cast<int>(results.size()), 1);
    TS_ASSERT_EQUALS(results[0], "foobar");
}


/**
 * Tests splitToRows function.
 */
void
StringToolsTest::testSplitToRows() {

    string testString = "something";

    string result = StringTools::splitToRows(testString, 3);
    TS_ASSERT_EQUALS(result, "som\neth\ning");

    result = StringTools::splitToRows(testString, 1);
    TS_ASSERT_EQUALS(result, "s\no\nm\ne\nt\nh\ni\nn\ng");

    result = StringTools::splitToRows(testString, 20);
    TS_ASSERT_EQUALS(result, testString);

    testString = "";
    result = StringTools::splitToRows(testString, 10);
    TS_ASSERT_EQUALS(result, testString);
}

/**
 * Tests endsWith().
 */
void 
StringToolsTest::testEndsWith() {
    const string searchString1 = "foooo/barfreX";
    const string searchString2 = "";
    const string searchString3 = "xX";

    TS_ASSERT(StringTools::endsWith(searchString1, searchString1));
    TS_ASSERT(StringTools::endsWith(searchString1, "X"));
    TS_ASSERT(!StringTools::endsWith(searchString2, "X"));
    TS_ASSERT(StringTools::endsWith(searchString2, ""));
    TS_ASSERT(StringTools::endsWith(searchString3, ""));
}

void
StringToolsTest::replaceString() {
    TCEString foo = "foo_bar__baz";
    TS_ASSERT_EQUALS(foo.replaceString("_", "\\_"), "foo\\_bar\\_\\_baz");
    TS_ASSERT_EQUALS(foo.replaceString("_", "."), "foo.bar..baz");
    TS_ASSERT_EQUALS(foo.replaceString("_", ".."), "foo..bar..baz");
    TS_ASSERT_EQUALS(foo.replaceString("_", ".."), "foo..bar....baz");   
}
#endif
