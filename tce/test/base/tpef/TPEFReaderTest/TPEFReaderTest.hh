/**
 * @file TPEFReaderTest.hh
 *
 * A test suite for TPEFReader and TPEFSectionReader classes.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 */

#ifndef TTA_TPEF_READER_TEST_HH
#define TTA_TPEF_READER_TEST_HH

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
    void testReadingTooShortFile();
    void testReadingJunkFile();
    
    void initStream();
    void testReadingTPEF();

    void setUp();
    void tearDown();
private:
    /// Stream for testing.
    static BinaryStream testStream_;
    /// Another stream for testing.
    static BinaryStream testStream2_;
    /// Tells if stream is already initialized.
    static bool streamInitialized_;
};


///////////////////////////////////////////////////////////////////////////////
// TPEFReaderTest implementation
///////////////////////////////////////////////////////////////////////////////
BinaryStream TPEFReaderTest::testStream_("data/temp.bin");
bool TPEFReaderTest::streamInitialized_ = false;

/**
 * Initialization code of each test
 */
void 
TPEFReaderTest::setUp() {    
    if (!streamInitialized_) {
      	initStream();
	streamInitialized_ = true;
    } 
}

/**
 * Cleanup code of each test.
 */
void 
TPEFReaderTest::tearDown() {
}

/**
 * Tries to read too short file.
 */
void 
TPEFReaderTest::testReadingTooShortFile() {
    BinaryStream testStream("data/almosttpefmagic.bin");    
    TS_ASSERT_THROWS(BinaryReader::readBinary(testStream),InstanceNotFound);
}

/**
 * Tries to read junk file.
 */
void 
TPEFReaderTest::testReadingJunkFile() {
    BinaryStream testStream("data/junkbinary.bin");    
    TS_ASSERT_THROWS(BinaryReader::readBinary(testStream),InstanceNotFound);
}

/**
 * Writes test TPEF binary into stream.
 */
void 
TPEFReaderTest::initStream() {    
    const Word STRTAB_SECTION_ID = 200;
    const Word STRTAB_LENGTH = 16;
    const Word STRTAB_DATA_OFFSET = 200; //112 minimum
    const Word STRTAB_ELEMENT_SIZE = 1;

    const Word ASPACE_SECTION_ID = 201;
    const Word ASPACE_ELEMENT_SIZE = 8;
    const Word ASPACE_SECTION_LENGTH = 4*ASPACE_ELEMENT_SIZE;
    const Word ASPACE_DATA_OFFSET = 300; //STRTAB_DATA_OFFSET + 16 minimum

    const Word NUMBER_OF_SECTIONS = 3;

    // file header id
    for(int i = 0; i < TPEFHeaders::FH_ID_SIZE; i++) {
        testStream_.writeByte(TPEFHeaders::FH_ID_BYTES[i]);
    }
    
    // file header rest
    testStream_.writeByte(Binary::FA_TTA_TUT);
    testStream_.writeByte(Binary::FT_OBJSEQ);
    // offset to first section header (size of header)
    testStream_.writeWord(TPEFHeaders::FH_HEADER_SIZE);
    // size of this header
    testStream_.writeHalfWord(TPEFHeaders::FH_HEADER_SIZE);
    // size of section header
    testStream_.writeHalfWord(TPEFHeaders::SH_HEADER_SIZE);
    // number of section headers
    testStream_.writeHalfWord(NUMBER_OF_SECTIONS);
    // fileoffset to string table
    testStream_.writeWord(testStream_.writePosition() + sizeof(Word));
       
    // String section header, offset 26
    // name section offset
    testStream_.writeWord(1);
    testStream_.writeByte(Section::ST_STRTAB);
    testStream_.writeByte(0);
    // address in mem aux section -> must be zero
    testStream_.writeWord(0);
    // starting offset
    testStream_.writeWord(STRTAB_DATA_OFFSET);
    // section length
    testStream_.writeWord(STRTAB_LENGTH);
    // section id
    testStream_.writeHalfWord(STRTAB_SECTION_ID);
    // section address space if aux section -> zero
    testStream_.writeByte(0);
    // padding
    testStream_.writeByte(0);
    // section link (must be zero for string table)
    testStream_.writeHalfWord(0);
    // info field must be zero if not used for section specific stuff
    testStream_.writeWord(0);
    // element size
    testStream_.writeWord(STRTAB_ELEMENT_SIZE);

    // Address space section header, offset 80
    // name section offset
    testStream_.writeWord(9);
    testStream_.writeByte(Section::ST_ADDRSP);
    testStream_.writeByte(0);
    // not program section -> 0
    testStream_.writeWord(0);
    // starting offset
    testStream_.writeWord(ASPACE_DATA_OFFSET);
    // section length
    testStream_.writeWord(ASPACE_SECTION_LENGTH);
    // section id
    testStream_.writeHalfWord(ASPACE_SECTION_ID);
    // sections address space entry
    testStream_.writeByte(0);
    // padding
    testStream_.writeByte(0);
    // section link (strtab id for aSpace section)
    testStream_.writeHalfWord(STRTAB_SECTION_ID);
    // info field...
    testStream_.writeWord(0);
    // element size
    testStream_.writeWord(ASPACE_ELEMENT_SIZE);

    // Null section header, offset 112
    testStream_.writeWord(0);
    testStream_.writeByte(Section::ST_NULL);
    testStream_.writeByte(0x80);
    // address in mem aux section -> must be zero
    testStream_.writeWord(0);
    // starting offset
    testStream_.writeWord(0);
    // section length
    testStream_.writeWord(0);
    // section id
    testStream_.writeHalfWord(0);
    // section address space if aux section -> zero
    testStream_.writeByte(0);
    // padding
    testStream_.writeByte(0);
    // section link (must be zero for string table)
    testStream_.writeHalfWord(0);
    // info field must be zero if not used for section specific stuff
    testStream_.writeWord(0);
    // element size
    testStream_.writeWord(0);

    // String section data
    while(testStream_.writePosition() < STRTAB_DATA_OFFSET) {
        testStream_.writeByte(0);
    }
    testStream_.setWritePosition(STRTAB_DATA_OFFSET);

    testStream_.writeByte(0);
    testStream_.writeByte('S');
    testStream_.writeByte('t');
    testStream_.writeByte('r');
    testStream_.writeByte('i');
    testStream_.writeByte('n');
    testStream_.writeByte('g');
    testStream_.writeByte('s');
    testStream_.writeByte(0);
    // section offset 9
    testStream_.writeByte('A');
    testStream_.writeByte('S');
    testStream_.writeByte('p');
    testStream_.writeByte('a');
    testStream_.writeByte('c');
    testStream_.writeByte('e');
    testStream_.writeByte(0);
    
    // Address space section data, offset 128
    while(testStream_.writePosition() < ASPACE_DATA_OFFSET) {
        testStream_.writeByte(0);
    }

    testStream_.setWritePosition(ASPACE_DATA_OFFSET);

    testStream_.writeByte(0);
    testStream_.writeByte(0);
    testStream_.writeByte(0);
    testStream_.writeByte(0);
    testStream_.writeWord(0);

    testStream_.writeByte(1);
    testStream_.writeByte(2);
    testStream_.writeByte(3);
    testStream_.writeByte(4);
    testStream_.writeWord(0);

    testStream_.writeByte(2);
    testStream_.writeByte(4);
    testStream_.writeByte(2);
    testStream_.writeByte(8);
    testStream_.writeWord(0);

    testStream_.writeByte(4);
    testStream_.writeByte(8);
    testStream_.writeByte(1);
    testStream_.writeByte(16);
    testStream_.writeWord(0);
}


/**
 * Reads test stream and checks that it was read right..
 */
void 
TPEFReaderTest::testReadingTPEF() {

    Binary* newBin = BinaryReader::readBinary(testStream_);
    // first we check that binary's info is fine
    TS_ASSERT_EQUALS(newBin->arch(), Binary::FA_TTA_TUT);
    TS_ASSERT_EQUALS(newBin->type(), Binary::FT_OBJSEQ);    
    StringSection* stringSection = newBin->strings();

    Word iter = 0;
    
    NullSection* nullSection = NULL;
    for (Word i = 0; i < newBin->sectionCount(); i++) {
	if (newBin->section(i)->type() == Section::ST_NULL) {
	    nullSection = dynamic_cast<NullSection*>(newBin->section(i));
	    break;
	}
    }
    
    ASpaceSection* aSpaceSection = NULL;
    // there is only two sections string and aspace, if first is not aspace
    // then second is
    if (newBin->section(iter)->type() != Section::ST_ADDRSP) {
        iter++;
    } 

    aSpaceSection = dynamic_cast<ASpaceSection*>
	(newBin->section(iter));

    TS_ASSERT_DIFFERS(stringSection, static_cast<Section*>(NULL));
    TS_ASSERT_DIFFERS(aSpaceSection, static_cast<Section*>(NULL));
    
    // string sections data
    TS_ASSERT_EQUALS(stringSection->type(), Section::ST_STRTAB);
    TS_ASSERT_EQUALS(stringSection->vLen(), false);
    TS_ASSERT_EQUALS(stringSection->noBits(), false);
    TS_ASSERT_EQUALS(static_cast<int>(stringSection->startingAddress()), 0);
    
    TS_ASSERT_EQUALS(stringSection->link(), static_cast<Section*>(nullSection));

    TS_ASSERT_EQUALS(stringSection->chunk2String(stringSection->name()), 
		     "Strings");    
      
    // aspace section data  
    TS_ASSERT_EQUALS(aSpaceSection->type(), Section::ST_ADDRSP);
    TS_ASSERT_EQUALS(aSpaceSection->vLen(), false);
    TS_ASSERT_EQUALS(aSpaceSection->noBits(), false);
    TS_ASSERT_EQUALS(static_cast<int>(aSpaceSection->startingAddress()), 0);
    
    TS_ASSERT_EQUALS(aSpaceSection->link(),
                     static_cast<Section*>(stringSection));

    TS_ASSERT_EQUALS(stringSection->chunk2String(aSpaceSection->name()), 
		     "ASpace");
    
    TS_ASSERT_EQUALS(static_cast<int>(aSpaceSection->elementCount()), 4);
    
    Word aSpaceIter = 0;

    ASpaceElement* aSpaceElement = dynamic_cast<ASpaceElement*>
	(aSpaceSection->element(aSpaceIter));

    TS_ASSERT_DIFFERS(aSpaceElement, static_cast<SectionElement*>(NULL));
    TS_ASSERT_EQUALS(aSpaceElement->MAU(), 0);
    TS_ASSERT_EQUALS(aSpaceElement->align(), 0);
    TS_ASSERT_EQUALS(aSpaceElement->wordSize(), 0);

    aSpaceIter++;
    aSpaceElement = dynamic_cast<ASpaceElement*>
	(aSpaceSection->element(aSpaceIter));
    TS_ASSERT_DIFFERS(aSpaceElement, static_cast<SectionElement*>(NULL));
    TS_ASSERT_EQUALS(aSpaceElement->MAU(), 2);
    TS_ASSERT_EQUALS(aSpaceElement->align(), 3);
    TS_ASSERT_EQUALS(aSpaceElement->wordSize(), 4);

    aSpaceIter++;
    aSpaceElement = dynamic_cast<ASpaceElement*>
	(aSpaceSection->element(aSpaceIter));
    TS_ASSERT_DIFFERS(aSpaceElement, static_cast<SectionElement*>(NULL));
    TS_ASSERT_EQUALS(aSpaceElement->MAU(), 4);
    TS_ASSERT_EQUALS(aSpaceElement->align(), 2);
    TS_ASSERT_EQUALS(aSpaceElement->wordSize(), 8);
    
    aSpaceIter++;
    aSpaceElement = dynamic_cast<ASpaceElement*>
	(aSpaceSection->element(aSpaceIter));
    TS_ASSERT_DIFFERS(aSpaceElement, static_cast<SectionElement*>(NULL));
    TS_ASSERT_EQUALS(aSpaceElement->MAU(), 8);
    TS_ASSERT_EQUALS(aSpaceElement->align(), 1);
    TS_ASSERT_EQUALS(aSpaceElement->wordSize(), 16);
    
    delete newBin;
    newBin = NULL;
}

#endif
