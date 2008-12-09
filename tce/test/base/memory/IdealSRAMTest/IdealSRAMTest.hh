/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file IdealSRAMTest.hh
 * 
 * A test suite for IdealSRAM.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
