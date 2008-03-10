/** 
 * @file StringSectionTest.hh 
 *
 * A test suite for StringSection.
 *
 * @author Ari Metsähalme (ari.metsahalme@tut.fi)
 * @note reviewed 17 October 2003 by am, pj, rm, kl
 */

#ifndef StringSectionTest_HH
#define StringSectionTest_HH

#include <iostream>
using std::cout;
using std::endl;

#include <string>
using std::string;

#include <TestSuite.h>
#include "StringSection.hh"
#include "BinaryStream.hh"
#include "Conversion.hh"
#include "Exception.hh"

using namespace TPEF;

/**
 * Test class for string section.
 */
class StringSectionTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testType();
    void testChunk2String();

private:
    /// Test string section.
    StringSection* strSect_;

    /// Name of the test input file.
    static const string fileName_;
    /// The length of file.
    static const unsigned int fileSize_;

    /// An offset pointing to a zero byte.
    static const unsigned int zeroOffset_;
    /// An offset which leads to eof with no terminating zero found.
    static const unsigned int errorOffset_;

    /// String to be found in the beginning of file.
    static const string refString_;
};


/**
 * Called before each test.
 */
void
StringSectionTest::setUp() {
    strSect_ = dynamic_cast<StringSection*>(
	Section::createSection(Section::ST_STRTAB));
}


/**
 * Called after each test.
 */
void
StringSectionTest::tearDown() {
    delete strSect_;
    strSect_ = NULL;
}


/**
 * Tests that type() returns correct section type.
 */
void
StringSectionTest::testType() {
    TS_ASSERT_EQUALS(strSect_->type(), Section::ST_STRTAB);
}


/**
 * Tests that chunk2String returns correct strings and throws
 * correct exception in an error situation.
 */
void
StringSectionTest::testChunk2String() {

    BinaryStream stream(fileName_);
   
    try {
	for (unsigned int i = 0;i < fileSize_; i++) {
	    strSect_->addByte(stream.readByte());
	}
    
	Chunk firstChunk(0);
	Chunk emptyChunk(zeroOffset_);
	Chunk errorChunk(errorOffset_);

	TS_ASSERT_EQUALS(strSect_->chunk2String(&firstChunk), refString_);
	TS_ASSERT_EQUALS(strSect_->chunk2String(&emptyChunk), "");
	TS_ASSERT_THROWS(strSect_->chunk2String(&errorChunk), UnexpectedValue);

    } catch (const UnreachableStream& error) {
	TS_FAIL("File not found");
    } catch (const IOException& error) {
	TS_FAIL("Error in binary file");
    }
}

const string StringSectionTest::fileName_ = "data/strsecttest.out";
const unsigned int StringSectionTest::fileSize_ = 0x1c;
const unsigned int StringSectionTest::zeroOffset_ = 0x0e;
const unsigned int StringSectionTest::errorOffset_ = 0x17;
const string StringSectionTest::refString_ = "joo.cc";

#endif
