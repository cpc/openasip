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
