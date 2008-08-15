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
#include <time.h>
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
    Memory::MAU data = 0xFEFEFEFE;
    Memory::MAU data2 = 0x00000000;
    Memory::MAUTable dataIn = &data;
    Memory::MAUTable dataOut = &data2;

    MemoryContents mem(addressSpaceSize);
    /// A geeky hack to produce a somewhat random seed ;)
    std::srand(time(NULL));
    
    for (size_t i = 0; i < accessCount; ++i) {
        const size_t address = 
            static_cast<size_t>(addressSpaceSize*(rand()*1.0/RAND_MAX));

        mem.write(address, dataIn, 1);
        mem.read(address, dataOut, 1);
        TS_ASSERT_EQUALS(data, dataOut[0]);
    }

    TS_ASSERT_LESS_THAN(mem.allocatedMemory(), accessCount*MEM_CHUNK_SIZE);
}


#endif
