/**
 * @file DataSectionTest.hh
 *
 * Tests for DataSection class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @note reviewed 21 October 2003 by rl, ml, jn, pj
 */

#ifndef TTA_DATA_SECTION_TEST_HH
#define TTA_DATA_SECTION_TEST_HH

#include <iostream>
using std::cout;
using std::endl;

#include <TestSuite.h>
#include "Section.hh"
#include "DataSection.hh"
#include "ASpaceSection.hh"
#include "Chunk.hh"
#include "BaseType.hh"

using namespace TPEF;

/**
 * Test class for data section.
 */
class DataSectionTest : public CxxTest::TestSuite {
public:
    void testChunkMethod(); 
    void setUp();
    void tearDown();

private:
    static DataSection *testSection1_;
    static DataSection *testSection2_;
    static DataSection *testSection3_;

    static const int sect1MAUCount_;
    static const int sect1MAUBits_;
    static const int sect2MAUCount_;
    static const int sect2MAUBits_;
    static const int sect3MAUCount_;
    static const int sect3MAUBits_;
};

/**
 * Initializes three data sections with different 
 * address spaces and initializes sections with data.
 */
inline void
DataSectionTest::setUp() {
    
    ASpaceElement *aSpace1 = new ASpaceElement();
    ASpaceElement *aSpace2 = new ASpaceElement();
    ASpaceElement *aSpace3 = new ASpaceElement();

    aSpace1->setMAU(sect1MAUBits_);
    aSpace2->setMAU(sect2MAUBits_);
    aSpace3->setMAU(sect3MAUBits_);

    testSection1_ = dynamic_cast<DataSection*>(
	Section::createSection(Section::ST_DATA));

    testSection2_ = dynamic_cast<DataSection*>(
	Section::createSection(Section::ST_DATA));
    
    testSection3_ = dynamic_cast<DataSection*>(
	Section::createSection(Section::ST_DATA));

    testSection1_->setASpace(aSpace1);
    testSection2_->setASpace(aSpace2);
    testSection3_->setASpace(aSpace3);
    
    int sect1BytesPerMAU = static_cast<int>(
	ceil(static_cast<double>(sect1MAUBits_) / BYTE_BITWIDTH));

    int sect2BytesPerMAU = static_cast<int>(
	ceil(static_cast<double>(sect2MAUBits_) / BYTE_BITWIDTH));

    int sect3BytesPerMAU = static_cast<int>(
	ceil(static_cast<double>(sect3MAUBits_) / BYTE_BITWIDTH));
    
    // create enough bytes to write MAUs
    for (int i = 0; i < sect1BytesPerMAU * sect1MAUCount_; i++) {
	testSection1_->addByte(0);
    }

    for (int i = 0; i < sect2BytesPerMAU * sect2MAUCount_; i++) {
	testSection2_->addByte(0);
    }

    for (int i = 0; i < sect3BytesPerMAU * sect3MAUCount_; i++) {
	testSection3_->addByte(0);
    }

    TS_ASSERT_DIFFERS(testSection1_, static_cast<SafePointable*>(NULL));
    TS_ASSERT_DIFFERS(testSection2_, static_cast<SafePointable*>(NULL));
    TS_ASSERT_DIFFERS(testSection3_, static_cast<SafePointable*>(NULL));
}

/**
 * Deletes created sections and elements.
 */
inline void
DataSectionTest::tearDown() {
    if (testSection1_ != NULL) {
	delete testSection1_->aSpace();
	delete testSection1_;
	testSection1_ = NULL;
    }

    if (testSection2_ != NULL) {
	delete testSection2_->aSpace();
	delete testSection2_;
	testSection2_ = NULL;
    }

    if (testSection3_ != NULL) {
	delete testSection3_->aSpace();
	delete testSection3_;
	testSection3_ = NULL;
    }
}

 /**
 * Tests if chunk method returns chunk object and that 
 * if chunk is asked twice with same offset it returns same 
 * chunk instance.
 */
inline void
DataSectionTest::testChunkMethod() {

    DataSection *dataSection =
        dynamic_cast<DataSection*>(
	    Section::createSection(Section::ST_DATA));

    unsigned int length = 2500;
    Byte* dataBytes = new Byte[length];

    assert(dataBytes != NULL);

    for (unsigned int i=0;i < length;i++) {
	dataSection->addByte(dataBytes[i]);
    }
    
    Chunk* testChunk = NULL, *testChunk2 = NULL, *testChunk3 = NULL;
    
    TS_ASSERT_THROWS_NOTHING(testChunk = dataSection->chunk(2000));
    TS_ASSERT_THROWS_NOTHING(testChunk2 = dataSection->chunk(0));
    
    TS_ASSERT_DIFFERS(testChunk, static_cast<SafePointable*>(NULL));
    TS_ASSERT_DIFFERS(testChunk2, static_cast<SafePointable*>(NULL));
    
    TS_ASSERT_EQUALS(static_cast<int>(testChunk->offset()), 2000);
    TS_ASSERT_EQUALS(static_cast<int>(testChunk2->offset()), 0);
    
    TS_ASSERT_THROWS_NOTHING(testChunk3 = dataSection->chunk(2000));
    TS_ASSERT(testChunk3 == testChunk);
                     

    delete dataSection;
    dataSection = NULL;
    delete[] dataBytes;
}

DataSection* DataSectionTest::testSection1_ = NULL;
DataSection* DataSectionTest::testSection2_ = NULL;
DataSection* DataSectionTest::testSection3_ = NULL;

const int DataSectionTest::sect1MAUCount_ = 200;
const int DataSectionTest::sect1MAUBits_ = 3;

const int DataSectionTest::sect2MAUCount_ = 200;
const int DataSectionTest::sect2MAUBits_ = 8;

const int DataSectionTest::sect3MAUCount_ = 200;
const int DataSectionTest::sect3MAUBits_ = 29;

#endif
