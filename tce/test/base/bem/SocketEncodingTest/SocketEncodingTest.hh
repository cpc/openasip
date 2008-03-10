/**
 * @file SocketEncodingTest.hh
 *
 * A test suite for SocketEncoding.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef SocketEncodingTest_HH
#define SocketEncodingTest_HH

#include <string>

#include <TestSuite.h>
#include "BinaryEncoding.hh"
#include "SocketEncoding.hh"
#include "MoveSlot.hh"
#include "SourceField.hh"
#include "BridgeEncoding.hh"
#include "NullSocketCodeTable.hh"

using std::string;

/**
 * Test suite for testing SocketEncoding class.
 */
class SocketEncodingTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testCreation();
    void testSocketCodes();

private:
    BinaryEncoding* bem_;
    SourceField* sField_;
};


/**
 * Called before each test.
 */
void
SocketEncodingTest::setUp() {
    bem_ = new BinaryEncoding();
    MoveSlot* slot = new MoveSlot("b1", *bem_);
    sField_ = new SourceField(BinaryEncoding::RIGHT, *slot);
}


/**
 * Called after each test.
 */
void
SocketEncodingTest::tearDown() {
    delete bem_;
}


/**
 * Tests the constructor of SocketEncoding class.
 */
void
SocketEncodingTest::testCreation() {
    
    const string s1 = "s1";
    const string s2 = "s2";
    const string s3 = "s3";
    
    SocketEncoding* enc1 = new SocketEncoding(s1, 0, 0, *sField_);
    TS_ASSERT(enc1->parent() == sField_);
    TS_ASSERT(sField_->socketEncodingCount() == 1);
    TS_ASSERT(enc1->socketName() == s1);
    TS_ASSERT(enc1->encoding() == 0);

    SocketEncoding* enc2 = new SocketEncoding(s2, 1, 0, *sField_);
    TS_ASSERT(sField_->socketEncodingCount() == 2);
    TS_ASSERT_THROWS(
	new SocketEncoding(s1, 2, 0, *sField_), ObjectAlreadyExists);
    TS_ASSERT_THROWS(
	new SocketEncoding(s3, 1, 0, *sField_), ObjectAlreadyExists);

    delete enc1;
    delete enc2;

    new BridgeEncoding("br1", 2, 0, *sField_);
    TS_ASSERT_THROWS(
	new SocketEncoding(s3, 2, 0, *sField_), ObjectAlreadyExists);

    TS_ASSERT(sField_->socketEncodingCount() == 0);
}


/**
 * Tests setting and unsetting socket code table.
 */
void
SocketEncodingTest::testSocketCodes() {

    SocketEncoding* enc1 = new SocketEncoding("s1", 0, 0, *sField_);
    TS_ASSERT(!enc1->hasSocketCodes());
    TS_ASSERT(&enc1->socketCodes() == &NullSocketCodeTable::instance());
    
    TS_ASSERT(enc1->socketIDWidth() == 1);
    TS_ASSERT(enc1->extraBits() == 0);
    TS_ASSERT(enc1->width() == 1);

    SocketCodeTable* table = new SocketCodeTable("t1", *bem_);
    enc1->setSocketCodes(*table);
    TS_ASSERT(enc1->hasSocketCodes());
    TS_ASSERT(&enc1->socketCodes() == table);
    TS_ASSERT(enc1->width() == 1);
    
}

#endif
