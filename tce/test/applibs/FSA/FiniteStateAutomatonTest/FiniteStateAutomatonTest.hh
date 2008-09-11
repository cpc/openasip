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
 * @file FiniteStateAutomatonTest.hh
 *
 * A test suite for the finite state automaton (FSA) classes.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
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
