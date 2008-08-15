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

    void testBasicInterface();

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
const Word IdealSRAMTest::MAUSIZE = 8;
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
 * Tests that the basic interface works and that the writes are commited
 * at the cycle advance.
 */
void
IdealSRAMTest::testBasicInterface() {
    
    IdealSRAM memory(START, END, MAUSIZE);

    UIntWord result;

    memory.read(200, 1, result);

    TS_ASSERT_EQUALS(result, static_cast<UIntWord>(0));

    // write data to memory
    for (std::size_t i = 0; i < 5; ++i) {
        memory.write(100 + i, 1, 128);
    }

    // assert the data is not yet commited to the memory
    for (std::size_t i = 0; i < 5; ++i) {
        memory.read(100 + i, 1, result);
        TS_ASSERT_EQUALS(result, static_cast<UIntWord>(0));
    }

    // now it should be commited
    memory.advanceClock();

    // assert the data is commited to the memory
    for (std::size_t i = 0; i < 5; ++i) {
        memory.read(100 + i, 1, result);
        TS_ASSERT_EQUALS(result, static_cast<UIntWord>(128));
    }

    // test the FloatWord and DoubleWord interface
    DoubleWord d = 123.123;
    memory.write(100, d);
    memory.advanceClock();

    memory.read(100, d);
    TS_ASSERT_DELTA(d, 123.123, 0.1);
}


#endif
