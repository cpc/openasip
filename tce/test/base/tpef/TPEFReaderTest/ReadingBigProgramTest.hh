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
 * @file ReadingBigProgramTest.hh
 *
 * A test suite for TPEFReader and TPEFSectionReader classes.
 * Test is designed to help profiling bottlenecks in system.
 *
 * @author Mikael Lepistö 2006 (tmlepist-no.spam-cs.tut.fi)
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
