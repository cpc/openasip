/**
 * @file FiniteStateAutomatonTest.hh
 *
 * A test suite for the finite state automaton (FSA) classes.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef FSA_TEST_HH
#define FSA_TEST_HH

#include <TestSuite.h>
#include "FiniteStateAutomaton.hh"
#include "hash_map.hh"

class FiniteStateAutomatonTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testBasicUsage();
};


void 
FiniteStateAutomatonTest::setUp() {
}

void 
FiniteStateAutomatonTest::tearDown() {
}

/**
 * Tests the basic usage of the methods in the base class FiniteStateAutomaton.
 */
void
FiniteStateAutomatonTest::testBasicUsage() {

    FiniteStateAutomaton fsa;
    TS_ASSERT_EQUALS(fsa.addState(), 0);
    TS_ASSERT_EQUALS(fsa.addState(), 1);
    TS_ASSERT_EQUALS(fsa.addState(), 2);

    TS_ASSERT_EQUALS(fsa.addTransitionType("A"), 0);
    TS_ASSERT_EQUALS(fsa.addTransitionType("B"), 1);
    TS_ASSERT_EQUALS(fsa.addTransitionType("C"), 2);
    TS_ASSERT_EQUALS(fsa.addTransitionType("D"), 3);

    TS_ASSERT_EQUALS(fsa.transitionName(0), "A");
    TS_ASSERT_EQUALS(fsa.transitionName(1), "B");
    TS_ASSERT_EQUALS(fsa.transitionName(2), "C");
    TS_ASSERT_EQUALS(fsa.transitionName(3), "D");
    TS_ASSERT_THROWS(fsa.transitionName(4), std::out_of_range);

    TS_ASSERT_EQUALS(fsa.transitionIndex("A"), 0);
    TS_ASSERT_EQUALS(fsa.transitionIndex("B"), 1);
    TS_ASSERT_EQUALS(fsa.transitionIndex("C"), 2);
    TS_ASSERT_EQUALS(fsa.transitionIndex("D"), 3);
    TS_ASSERT_THROWS(fsa.transitionIndex("X"), KeyNotFound);

    fsa.setTransition(0, 1, 0);
    fsa.setTransition(0, 2, 1);
    fsa.setTransition(2, 0, 2);

    TS_ASSERT(fsa.isLegalTransition(0, 0));
    TS_ASSERT(fsa.isLegalTransition(0, 1));
    TS_ASSERT(!fsa.isLegalTransition(1, 2));

    TS_ASSERT_EQUALS(fsa.destinationState(2, 2), 0);
}

#endif
