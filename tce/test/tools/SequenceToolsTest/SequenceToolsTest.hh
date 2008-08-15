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
 * @file SequenceToolsTest.hh
 *
 * A test suite for SequenceTools class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
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
