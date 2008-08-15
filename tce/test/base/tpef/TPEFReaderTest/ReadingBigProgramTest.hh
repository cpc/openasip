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
 * @file ReadingBigProgramTest.hh
 *
 * A test suite for TPEFReader and TPEFSectionReader classes.
 * Test is designed to help profiling bottlenecks in system.
 *
 * @author Mikael Lepistö 2006 (tmlepist@cs.tut.fi)
 */

#ifndef TTA_READING_BIG_PROGRAM_TEST_HH
#define TTA_READING_BIG_PROGRAM_TEST_HH

#include <list>
#include <TestSuite.h>

#include "Section.hh"
#include "BinaryReader.hh"
#include "BinaryStream.hh"
#include "Binary.hh"
#include "StringSection.hh"
#include "ASpaceSection.hh"
#include "ASpaceElement.hh"
#include "TPEFHeaders.hh"
#include "NullSection.hh"

using namespace TPEF;

///////////////////////////////////////////////////////////////////////////////
// TPEFReaderTest declaration
///////////////////////////////////////////////////////////////////////////////

/**
 * Tests for TPEFReader and TPEFSectionReader classes.
 */
class TPEFReaderTest : public CxxTest::TestSuite {
public:
    void testReadingTPEF();
    void setUp();
    void tearDown();
private:
    /// Stream for testing.
    static BinaryStream testStream1_;
    static BinaryStream testStream2_;
};


///////////////////////////////////////////////////////////////////////////////
// TPEFReaderTest implementation
///////////////////////////////////////////////////////////////////////////////
BinaryStream TPEFReaderTest::testStream1_("data/tremor");
BinaryStream TPEFReaderTest::testStream2_("data/tremor.tpef");

/**
 * Initialization code of each test
 */
void 
TPEFReaderTest::setUp() {    
}

/**
 * Cleanup code of each test.
 */
void 
TPEFReaderTest::tearDown() {
}

/**
 * Reads test stream,
 */
void 
TPEFReaderTest::testReadingTPEF() {
    Binary* newBin1 = BinaryReader::readBinary(testStream1_);
    Binary* newBin2 = BinaryReader::readBinary(testStream2_);
    delete newBin1;
    delete newBin2;
}

#endif
