/**
 * @file BasicElementTest.hh
 *
 * Tests for BasicElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 */

#ifndef TTA_BASIC_ELEMENT_TEST_HH
#define TTA_BASIC_ELEMENT_TEST_HH

#include <TestSuite.h>
#include <vector>

#include "BasicElement.hh"

using std::vector;
using namespace TPEF;

/**
 * Tests for BasicElement class.
 */
class BasicElementTest : public CxxTest::TestSuite {
public:
    void testSetAndGet();

    void setUp();
    void tearDown();

private:
    static const Word DATA_LENGTH;

    /// Test element.
    static BasicElement element_;
    /// Test data.
    static vector<Byte> bytes_;
};

BasicElement BasicElementTest::element_;
vector<Byte> BasicElementTest::bytes_;
const Word BasicElementTest::DATA_LENGTH = 1000;

/**
 * Tests setBytes and getBytes methods.
 */ 
void 
BasicElementTest::testSetAndGet() {
    for (Word i = 0; i < bytes_.size(); i++) {
	element_.addByte(bytes_[i]);
    }
    
    // vector where to collect data from element
    vector<Byte> temp;
    for(Word i = 0; i < element_.length(); i++) {
	temp.push_back(element_.byte(i));
    }
    
    TS_ASSERT(temp == bytes_);
}

/**
 * Creates test data.
 */
void 
BasicElementTest::setUp() {
    for (Word i = 0;i < DATA_LENGTH;i++) {
	bytes_.push_back(static_cast<Byte>(i%256));
    }
}

/**
 * Test suite's cleanup method.
 */
void 
BasicElementTest::tearDown() {
}

#endif
