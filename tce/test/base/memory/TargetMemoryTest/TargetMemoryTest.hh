/** 
 * @file TargetMemoryTest.hh
 * 
 * A test suite for TargetMemory.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TARGET_MEMORY_TEST_HH
#define TARGET_MEMORY_TEST_HH

#include <TestSuite.h>

#include "TargetMemory.hh"
#include "IdealSRAM.hh"

/**
 * Test class for TargetMemory class.
 */
class TargetMemoryTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testReadWrite();
    void testReadBlockWriteBlock();
    void testExoticMAUSize();

private:
    static const Word START;
    static const Word END;
    static const Word MAUSIZE;
    static const Word WORDSIZE;
    static const int ALIGNMENT;
};

const Word TargetMemoryTest::START = 200;
const Word TargetMemoryTest::END = 2000;
const Word TargetMemoryTest::MAUSIZE = 8;
const Word TargetMemoryTest::WORDSIZE = 4;
const int TargetMemoryTest::ALIGNMENT = 2;

/**
 * Called before each test.
 */
void
TargetMemoryTest::setUp() {
}


/**
 * Called after each test.
 */
void
TargetMemoryTest::tearDown() {
}

/**
 * Tests that reading and writing works.
 */
void
TargetMemoryTest::testReadWrite() {

    IdealSRAM memory(START, END, MAUSIZE, WORDSIZE, ALIGNMENT);
    TargetMemory target(memory, true, MAUSIZE);

    UIntWord intData = 69;
    target.initiateWrite(250, 4, intData, 1);
    target.advanceClock();

    target.initiateRead(250, 4, 1);
    
    UIntWord intResult = 0;
    target.readData(intResult, 1);
    TS_ASSERT_EQUALS(static_cast<int>(intResult), 69);
    
    FloatWord floatData = 6.9;
    target.initiateWrite(300, 4, floatData, 1);
    target.advanceClock();

    target.initiateRead(300, 4, 1);

    FloatWord floatResult = 0;
    target.readData(floatResult, 1);
    TS_ASSERT_DELTA(floatResult, 6.9, 0.0001);

    DoubleWord doubleData = 1234.12;

    target.initiateWrite(400, 8, doubleData, 1);
    target.advanceClock();

    target.initiateRead(400, 8, 1);

    DoubleWord doubleResult = 0;
    target.readData(doubleResult, 1);

    TS_ASSERT_DELTA(doubleResult, 1234.12, 0.0001);

}

/**
 * Tests that reading and writing blocks works.
 */
void
TargetMemoryTest::testReadBlockWriteBlock() {

    IdealSRAM memory(START, END, MAUSIZE, WORDSIZE, ALIGNMENT);
    TargetMemory target(memory, true, MAUSIZE);

    TargetMemory::UIntWordVector ints(2, 69);
    target.writeBlock(300, ints, 32);

    TargetMemory::UIntWordVector intResults;
    intResults.resize(2);
    target.readBlock(300, intResults, 32);

    TS_ASSERT_EQUALS(static_cast<int>(intResults[0]), 69);
    TS_ASSERT_EQUALS(static_cast<int>(intResults[1]), 69);

    TargetMemory::FloatWordVector floats(2, 3.2);
    target.writeBlock(350, floats, 32);

    TargetMemory::FloatWordVector floatResults;
    floatResults.resize(2);
    target.readBlock(350, floatResults, 32);

    TS_ASSERT_DELTA(floatResults[0], 3.2, 0.0001);
    TS_ASSERT_DELTA(floatResults[1], 3.2, 0.0001);

    TargetMemory::DoubleWordVector doubles(2, 12345.12);
    target.writeBlock(450, doubles, 64);

    TargetMemory::DoubleWordVector doubleResults;
    doubleResults.resize(2);
    target.readBlock(450, doubleResults, 64);
    
    TS_ASSERT_DELTA(doubleResults[0], 12345.12, 0.01);
    TS_ASSERT_DELTA(doubleResults[1], 12345.12, 0.01);
}

/**
 * Test for a situation when size of MAU is not a byte.
 */
void
TargetMemoryTest::testExoticMAUSize() {

    IdealSRAM memory(START, END, 12, WORDSIZE, ALIGNMENT);
    TargetMemory target(memory, true, 12);

    UIntWord intData = 178293;
    target.initiateWrite(200, 2, intData, 1);
    target.advanceClock();
    
    UIntWord intResult = 0;
    target.initiateRead(200, 2, 1);
    target.readData(intResult, 1);
    
    TS_ASSERT_EQUALS(static_cast<int>(intResult), 178293);

    FloatWord floatData = 123.123;
    target.initiateWrite(300, 3, floatData, 1);
    target.advanceClock();

    FloatWord floatResult = 0;
    target.initiateRead(300, 3, 1);
    target.readData(floatResult, 1);

    TS_ASSERT_DELTA(floatResult, 123.123, 0.001);

    DoubleWord doubleData = 12.12;
    target.initiateWrite(400, 6, doubleData, 1);
    target.advanceClock();

    TargetMemory::UIntWordVector ints(5, 122343213);
    target.writeBlock(450, ints, 32);

    TargetMemory::UIntWordVector intResults;
    intResults.resize(5);
    target.readBlock(450, intResults, 32);

    TS_ASSERT_EQUALS(static_cast<int>(intResults[0]), 122343213);
    TS_ASSERT_EQUALS(static_cast<int>(intResults[1]), 122343213);
    TS_ASSERT_EQUALS(static_cast<int>(intResults[2]), 122343213);
    TS_ASSERT_EQUALS(static_cast<int>(intResults[3]), 122343213);
    TS_ASSERT_EQUALS(static_cast<int>(intResults[4]), 122343213);

    TargetMemory::FloatWordVector floats(5, 1234.12);
    target.writeBlock(500, floats, 32);

    TargetMemory::FloatWordVector floatResults;
    floatResults.resize(5);
    target.readBlock(500, floatResults, 32);
    
    TS_ASSERT_DELTA(floatResults[0], 1234.12, 0.01);
    TS_ASSERT_DELTA(floatResults[1], 1234.12, 0.01);
    TS_ASSERT_DELTA(floatResults[2], 1234.12, 0.01);
    TS_ASSERT_DELTA(floatResults[3], 1234.12, 0.01);
    TS_ASSERT_DELTA(floatResults[4], 1234.12, 0.01);

    TargetMemory::DoubleWordVector doubles(5, 123453.123);
    target.writeBlock(550, doubles, 64);
}

#endif
