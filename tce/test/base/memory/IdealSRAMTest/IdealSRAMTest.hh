/** 
 * @file IdealSRAMTest.hh
 * 
 * A test suite for IdealSRAM.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#ifndef IDEAL_SRAM_TEST_HH
#define IDEAL_SRAM_TEST_HH

#include <TestSuite.h>

#include "IdealSRAM.hh"

/**
 * Class for testing IdealSRAM.
 */
class IdealSRAMTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testLoad();
    void testMultipleLoadRequests();
    void testReadWriteBlock();

private:
    /// Starting point of the memory.
    static const Word START;
    /// End point of the memory.
    static const Word END;
    /// Size of the minimum addressable unit (MAU).
    static const Word MAUSIZE;
    /// Size of the word as MAUs.
    static const Word WORDSIZE;
    /// Alignment constraint.
    static const int ALIGNMENT;
};

const Word IdealSRAMTest::START = 100;
const Word IdealSRAMTest::END = 1000;
const Word IdealSRAMTest::MAUSIZE = 16;
const Word IdealSRAMTest::WORDSIZE = 2;
const int IdealSRAMTest::ALIGNMENT = 2;


/**
 * Called before each test.
 */
void
IdealSRAMTest::setUp() {
}


/**
 * Called after each test.
 */
void
IdealSRAMTest::tearDown() {
}

/**
 * Tests that load works.
 */
void
IdealSRAMTest::testLoad() {
    
    IdealSRAM memory(START, END, MAUSIZE, WORDSIZE, ALIGNMENT);

    memory.initiateRead(200, 8, 1);

    // Memory should be first all zeros
    Memory::MAUVector result;
    
    memory.loadData(result, 1);
    TS_ASSERT_EQUALS(static_cast<int>(result.size()), 8);
    TS_ASSERT_EQUALS(result[0], 0);
    TS_ASSERT_EQUALS(result[7], 0);

    result.clear();

    // write data to memory
    Memory::MAU data[5];
    for (std::size_t i = 0; i < 5; ++i) {
        data[i] = 128;
    }
   
    memory.initiateWrite(200, data, 5, 1);

    memory.advanceClock();

    memory.initiateRead(200, 5, 1);
    memory.loadData(result, 1);
    TS_ASSERT_EQUALS(static_cast<int>(result.size()), 5);
    TS_ASSERT_EQUALS(result[0], 128);
    TS_ASSERT_EQUALS(result[1], 128);
    TS_ASSERT_EQUALS(result[2], 128);
    TS_ASSERT_EQUALS(result[3], 128);
    TS_ASSERT_EQUALS(result[4], 128);
}

/**
 * Test that multiple load requests work.
 */
void
IdealSRAMTest::testMultipleLoadRequests() {
    
    IdealSRAM memory(START, END, MAUSIZE, WORDSIZE, ALIGNMENT);

    // let's write things to memory
    Memory::MAU data1[10];
    for (std::size_t i = 0; i < 10; ++i) {
        data1[i] = 10;
    }

    Memory::MAU data2[10];
    for (std::size_t i = 0; i < 10; ++i) {
        data2[i] = 20;
    }

    Memory::MAU data3[10];
    for (std::size_t i = 0; i < 10; ++i) {
        data3[i] = 30;
    }


    TS_ASSERT_EQUALS(memory.resultReady(1), false);

    memory.initiateWrite(100, data1, 10, 1);
    memory.initiateWrite(200, data2, 10, 2);
    memory.initiateWrite(300, data3, 10, 3);

    memory.advanceClock();

    memory.initiateRead(100, 1, 1);
    memory.initiateRead(200, 1, 2);
    memory.initiateRead(300, 1, 3);

    TS_ASSERT_EQUALS(memory.resultReady(1), true);

    Memory::MAUVector result;
    memory.loadData(result, 1);
    TS_ASSERT_EQUALS(static_cast<int>(result.size()), 1);
    TS_ASSERT_EQUALS(result[0], 10);
    result.clear();
    
    memory.loadData(result, 2);
    TS_ASSERT_EQUALS(static_cast<int>(result.size()), 1);
    TS_ASSERT_EQUALS(result[0], 20);
    result.clear();

    memory.loadData(result, 3);
    TS_ASSERT_EQUALS(static_cast<int>(result.size()), 1);
    TS_ASSERT_EQUALS(result[0], 30);
}

/**
 * Test that reading and writing the block succeeds.
 */
void
IdealSRAMTest::testReadWriteBlock() {
    IdealSRAM memory(START, END, MAUSIZE, WORDSIZE, ALIGNMENT);

    Memory::MAUVector data(5, 648);
    memory.writeBlock(200, data);

    Memory::MAUVector result;
    result.resize(5);

    memory.readBlock(200, result);

    TS_ASSERT_EQUALS(static_cast<int>(result.size()), 5);
    TS_ASSERT_EQUALS(result[0], 648);
    TS_ASSERT_EQUALS(result[1], 648);
    TS_ASSERT_EQUALS(result[2], 648);
    TS_ASSERT_EQUALS(result[3], 648);
    TS_ASSERT_EQUALS(result[4], 648);
}

#endif
