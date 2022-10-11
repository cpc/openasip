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
 * @file FiniteStateAutomatonTest.hh
 *
 * A test suite for the finite state automaton (FSA) classes.
 *
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
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
