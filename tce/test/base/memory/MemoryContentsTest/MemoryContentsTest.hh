/** 
 * @file MemoryContentsTest.hh
 * 
 * A test suite for MemoryContents.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pjaaskel@cs.tut.fi)
 */

#ifndef MEMORY_CONTENTS_TEST_HH
#define MEMORY_CONTENTS_TEST_HH

#include <TestSuite.h>
#include <cstdlib>

#include "MemoryContents.hh"

/**
 * Class for testing MemoryContents.
 */
class MemoryContentsTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testStressTest();
};

/**
 * Called before each test.
 */
void
MemoryContentsTest::setUp() {
}


/**
 * Called after each test.
 */
void
MemoryContentsTest::tearDown() {
}

/**
 * Reads and writes randomly the memory.
 *
 * Allocates a 32bit address space.
 */
void
MemoryContentsTest::testStressTest() {
    
    const size_t accessCount = 1000;    
    const size_t addressSpaceSize = 0xFFFFFFFF;
    Memory::MAUVector dataOut;
    Memory::MAU data = 0xFEFEFEFE;
    Memory::MAUTable dataIn = &data;

    MemoryContents mem(addressSpaceSize);
    /// A geeky hack to produce a somewhat random seed ;)
    std::srand(reinterpret_cast<unsigned int>(this));
    
    for (size_t i = 0; i < accessCount; ++i) {
        const size_t address = 
            static_cast<size_t>(addressSpaceSize*(rand()*1.0/RAND_MAX));

        mem.write(address, dataIn, 1);
        mem.read(address, dataOut, 1);
        TS_ASSERT_EQUALS(data, dataOut.at(0));
    }

    TS_ASSERT_LESS_THAN(mem.allocatedMemory(), accessCount*MEM_CHUNK_SIZE);
}


#endif
