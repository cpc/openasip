/**
 * @file ValueReplacerTest.hh
 *
 * Tests for ValueReplacer inherited class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 */

#ifndef TTA_VALUE_REPLACER_TEST_HH
#define TTA_VALUE_REPLACER_TEST_HH

#include <string>
#include <TestSuite.h>

#include "ValueReplacer.hh"
#include "TPEFWriter.hh"
#include "BinaryStream.hh"
#include "SafePointer.hh"
#include "BaseType.hh"
#include "Exception.hh"

#include "FileOffsetReplacer.hh"
#include "SectionOffsetReplacer.hh"
#include "SectionIndexReplacer.hh"
#include "SectionSizeReplacer.hh"

using namespace TPEF;

using ReferenceManager::SafePointer;
using ReferenceManager::SectionOffsetKey;
using ReferenceManager::SectionIndexKey;
using ReferenceManager::FileOffsetKey;

/**
 * Tests for ValueRepalcer inherited classes.
 */
class ValueReplacerTest : public CxxTest::TestSuite {
public:
    void testSectionSizeReplacer();
    void testSectionIndexReplacer();
    void testMissingKeys();    
    void testReplacing();

    void setUp();
    void tearDown();
};

static const Word SECTION_SIZE = 2003;
static const Word FALSE_SECTION_SIZE = 4153;

// Words for ThisIsOk text file.
static const FileOffset FILE_OFFSET = 0x54686973;
static const SectionOffset SECTION_OFFSET = 0x49734f6b;

///////////////////////////////////////////////////////////////////////////////
// ValueReplacerTest implementation 
///////////////////////////////////////////////////////////////////////////////

/**
 * Tests SectionSizeReplacer setSize and clear methods.
 */
void 
ValueReplacerTest::testSectionSizeReplacer() {

    BinaryStream stream("data/test.bin");
    Chunk* safePointable = new Chunk(0);    
    
    // test clear
    SectionSizeReplacer::setSize(safePointable, FALSE_SECTION_SIZE);
    SectionSizeReplacer::clear();

    ValueReplacer::initialize(stream);

    // and set size
    SectionSizeReplacer replacer(safePointable);
    replacer.resolve();
    SectionSizeReplacer::setSize(safePointable, SECTION_SIZE);

    ValueReplacer::finalize();
    
    Word testWord = stream.readWord();
    TS_ASSERT_EQUALS(testWord, SECTION_SIZE);
    TS_ASSERT_DIFFERS(testWord, FALSE_SECTION_SIZE);
    
    delete safePointable;
    safePointable = NULL;
}

/**
 * Tests SectionIndexReplacer with different index sizes.
 */
void 
ValueReplacerTest::testSectionIndexReplacer() {

    BinaryStream stream("data/test1.bin");

    Chunk* safePointable1 = new Chunk(0);    
    Chunk* safePointable2 = new Chunk(10);    
    Chunk* safePointable3 = new Chunk(20);    
    Chunk* safePointable4 = new Chunk(30);    

    ValueReplacer::initialize(stream);
    
    // 1 byte index to stream
    SectionIndexReplacer replace1(safePointable1, 1);
    replace1.resolve();

    // 2 byte index to stream
    SectionIndexReplacer replace2(safePointable2, 2);
    replace2.resolve();

    // 1 byte index value key
    SectionIndexKey sKey1(0,0x12);
    SafePointer::addObjectReference(sKey1, safePointable1);

    // 3 byte index to stream
    SectionIndexReplacer replace3(safePointable3, 3);
    replace3.resolve();

    // 2 byte index value key
    SectionIndexKey sKey2(0,0x1234);
    SafePointer::addObjectReference(sKey2, safePointable2);

    // 3 byte index value key
    SectionIndexKey sKey3(0,0x123456);
    SafePointer::addObjectReference(sKey3, safePointable3);

    // 4 byte index value key
    SectionIndexKey sKey4(0,0x12345678);
    SafePointer::addObjectReference(sKey4, safePointable4);

    // 4 byte index to stream
    SectionIndexReplacer replace4(safePointable4, 4);
    replace4.resolve();

    ValueReplacer::finalize();

    TS_ASSERT_EQUALS(stream.readByte(), 0x12);
    TS_ASSERT_EQUALS(stream.readHalfWord(), 0x1234);

    // read 3 byte value from big endian byte order
    TS_ASSERT_EQUALS(stream.readHalfWord(), 0x3456);
    TS_ASSERT_EQUALS(stream.readByte(), 0x12);

    TS_ASSERT_EQUALS(static_cast<int>(stream.readWord()), 0x12345678);

    delete safePointable1;
    safePointable1 = NULL;
    delete safePointable2;
    safePointable2 = NULL;
    delete safePointable3;
    safePointable3 = NULL;
    delete safePointable4;
    safePointable4 = NULL;
}

/**
 * Tests MissingKeys exception.
 */
void 
ValueReplacerTest::testMissingKeys() {
    BinaryStream stream("data/test.bin");
    Chunk* safePointable = new Chunk(0);

    ValueReplacer::initialize(stream);

    FileOffsetReplacer replacer(safePointable);
    replacer.resolve();

    TS_ASSERT_THROWS(ValueReplacer::finalize(), MissingKeys);

    FileOffsetKey fOffKey(FILE_OFFSET);
    SafePointer::addObjectReference(fOffKey, safePointable);

    ValueReplacer::finalize();

    delete safePointable;
    safePointable = NULL;
}

/**
 * Tests to write straight replacement and delayed replacement.
 */
void 
ValueReplacerTest::testReplacing() {
    BinaryStream stream("data/test.bin");
    Chunk* safePointable = new Chunk(0);
    
    ValueReplacer::initialize(stream);

    // add first key
    FileOffsetKey fKey(FILE_OFFSET);
    SafePointer::addObjectReference(fKey, safePointable);

    // write references
    FileOffsetReplacer fOffsetReplacer(safePointable);
    fOffsetReplacer.resolve();

    SectionOffsetReplacer sOffsetReplacer(safePointable);
    sOffsetReplacer.resolve();

    // add second key
    SectionOffsetKey sKey(0,SECTION_OFFSET);
    SafePointer::addObjectReference(sKey, safePointable);

    // finalize
    ValueReplacer::finalize();

    // read stream
    TS_ASSERT_EQUALS(stream.readWord(), FILE_OFFSET);
    TS_ASSERT_EQUALS(stream.readWord(), SECTION_OFFSET);
    
    delete safePointable;
    safePointable = NULL;  
}

/**
 * Initialization code of test suite.
 */
void 
ValueReplacerTest::setUp() {
}

/**
 * Cleanup code of test suite.
 */
void 
ValueReplacerTest::tearDown() {
}

#endif
