/**
 * @file AssocToolsTest.hh
 *
 * A test suite for AssocTools class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
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
