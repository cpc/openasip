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
