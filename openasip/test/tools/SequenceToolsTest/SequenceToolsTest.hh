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
 * @file SequenceToolsTest.hh
 *
 * A test suite for SequenceTools class.
 *
 * @author Pekka Jääskeläinen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
 */

#ifndef TTA_SEQUENCETOOLS_TEST_HH
#define TTA_SEQUENCETOOLS_TEST_HH

#include <TestSuite.h>
#include "SequenceTools.hh"

#include <vector>
using std::vector;

/**
 * Implements the tests needed to verify correct operation of SequenceTools.
 */
class SequenceToolsTest : public CxxTest::TestSuite {
public:
    void testDeleteAllItems();
};


// A dummy class to verify deleteAllItems() really deletes the objects.
class Dummy {
public:
    // This counter is incremented in creation and decremented in deletion,
    // thus it should be zero in case no memory leak from this class.
    static int liveCount;
    Dummy() {
	liveCount++;
    }
    ~Dummy() {
	liveCount--;
    }
    
};

int Dummy::liveCount = 0;

/**
 * Test deleteAllItems.
 */
void 
SequenceToolsTest::testDeleteAllItems() {
    
    vector<Dummy*> firstVec;
    firstVec.push_back(new Dummy());
    firstVec.push_back(new Dummy());
    firstVec.push_back(new Dummy());
    firstVec.push_back(new Dummy());
    
    TS_ASSERT_EQUALS(Dummy::liveCount, 4);
    SequenceTools::deleteAllItems(firstVec);
    TS_ASSERT_EQUALS(static_cast<int>(firstVec.size()), 0);
    TS_ASSERT_EQUALS(Dummy::liveCount, 0);
    
}


#endif
