/**
 * @file BridgeEncodingTest.hh
 *
 * A test suite for BridgeEncoding.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef BridgeEncodingTest_HH
#define BridgeEncodingTest_HH

#include <string>

#include <TestSuite.h>
#include "BinaryEncoding.hh"
#include "BridgeEncoding.hh"
#include "MoveSlot.hh"
#include "SourceField.hh"

using std::string;

/**
 * Test suite for testing BridgeEncoding class.
 */
class BridgeEncodingTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();

private:
    BinaryEncoding* bem_;
    SourceField* sField_;
};


/**
 * Called before each test.
 */
void
BridgeEncodingTest::setUp() {
    bem_ = new BinaryEncoding();
    MoveSlot* slot = new MoveSlot("b1", *bem_);
    sField_ = new SourceField(BinaryEncoding::RIGHT, *slot);
}


/**
 * Called after each test.
 */
void
BridgeEncodingTest::tearDown() {
    delete bem_;
}


/**
 * Tests the constructor of BridgeEncoding class.
 */
void
BridgeEncodingTest::testCreation() {

    const string br1 = "br1";

    BridgeEncoding* enc1 = new BridgeEncoding(br1, 2, 0, *sField_);
    TS_ASSERT(enc1->parent() == sField_);
    TS_ASSERT(enc1->encoding() == 2);
    TS_ASSERT(enc1->bridgeName() == br1);
    TS_ASSERT(enc1->width() == 2);
}

#endif
