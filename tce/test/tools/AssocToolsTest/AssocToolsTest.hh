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
 * @file AssocToolsTest.hh
 *
 * A test suite for AssocTools class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
 */

#ifndef TTA_ASSOC_TOOLS_TEST_HH
#define TTA_ASSOC_TOOLS_TEST_HH

#include <TestSuite.h>
#include "AssocTools.hh"

#include <set>
#include <utility>

/**
 * Implements the tests needed to verify correct operation of AssocTools.
 */
class AssocToolsTest : public CxxTest::TestSuite {
public:
    void testPairs();
};

/**
 * Tests the pairs() function.
 */
void
AssocToolsTest::testPairs() {

    std::set<std::string> firstSet;
    firstSet.insert("a");
    firstSet.insert("b");

    std::set<int> secondSet;
    secondSet.insert(1);
    secondSet.insert(2);
    
    std::set<std::pair<std::string, int> > expected;
    expected.insert(std::make_pair("a", 1));
    expected.insert(std::make_pair("a", 2));
    expected.insert(std::make_pair("b", 1));
    expected.insert(std::make_pair("b", 2));

    std::set<std::pair<std::string, int> > pairs =
        AssocTools::pairs(firstSet, secondSet);
  
    TS_ASSERT_EQUALS(pairs, expected);
}

#endif
