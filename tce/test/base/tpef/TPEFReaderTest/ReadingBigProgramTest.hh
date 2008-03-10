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
