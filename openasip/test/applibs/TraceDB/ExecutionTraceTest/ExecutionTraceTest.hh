/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file ExecutionTraceTest.hh 
 *
 * A test suite for TraceDB.
 * 
 * @author Pekka Jääskeläinen 2004 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_TRACEDB_TEST_HH
#define TTA_TRACEDB_TEST_HH

#include <TestSuite.h>
#include <string>

#include "Exception.hh"
#include "ExecutionTrace.hh"
#include "InstructionExecution.hh"

using std::string;

/**
 * Class that tests ExecutionTrace and InstructionExecution class.
 */
class ExecutionTraceTest : public CxxTest::TestSuite {
public:
    ExecutionTraceTest();

    void setUp();
    void tearDown();

    void testInitialize();    
    void testAddInstructionExecution();
    void testInstructionExecution();
private:    
    ExecutionTrace* execTrace_;
};

const string nonexistingWritableDBFile = "data/new.tdb";

/**
 * Constructor.
 */
ExecutionTraceTest::ExecutionTraceTest() : 
    CxxTest::TestSuite(), execTrace_(NULL) {
}

/**
 * Called before each test.
 */
void
ExecutionTraceTest::setUp() {
}

/**
 * Called after each test.
 */
void
ExecutionTraceTest::tearDown() {
}

#if 0
#include <iostream>
using std::cerr;
using std::endl;
#include "Exception.hh"

#undef TS_ASSERT_THROWS_NOTHING
#define TS_ASSERT_THROWS_NOTHING(XX) \
  try { XX; } catch (const Exception& e) { cerr << e.errorMessage() << endl; }
#endif

/**
 * Tests that the tables are initialized correctly.
 */
void
ExecutionTraceTest::testInitialize() {
    TS_ASSERT_THROWS_NOTHING(
	execTrace_ = ExecutionTrace::open(nonexistingWritableDBFile));
}

/**
 * Tests addInstructionExecution().
 */
void 
ExecutionTraceTest::testAddInstructionExecution() {

    InstructionExecution& ie = execTrace_->instructionExecutions();

    TS_ASSERT_THROWS(ie.next(), NotAvailable);
    TS_ASSERT_THROWS(ie.cycle(), NotAvailable);
    TS_ASSERT_EQUALS(ie.hasNext(), false);

    TS_ASSERT_THROWS_NOTHING(execTrace_->addInstructionExecution(0, 1));
    InstructionExecution& ie2 = execTrace_->instructionExecutions();

    TS_ASSERT_THROWS_NOTHING(ie2.cycle());
    TS_ASSERT_THROWS(ie2.next(), NotAvailable);

    // insertion with an existing key
    TS_ASSERT_THROWS(execTrace_->addInstructionExecution(0, 1), IOException);

    TS_ASSERT_THROWS_NOTHING(execTrace_->addInstructionExecution(3, 14));
    TS_ASSERT_THROWS_NOTHING(execTrace_->addInstructionExecution(1, 1));
    TS_ASSERT_THROWS_NOTHING(execTrace_->addInstructionExecution(2, 3));
}

/**
 * Tests that traversing through the instruction_execution entries work using
 * the helper class InstructionExecution.
 */
void 
ExecutionTraceTest::testInstructionExecution() {

    InstructionExecution& ie = execTrace_->instructionExecutions();

    TS_ASSERT_EQUALS(ie.cycle(), static_cast<unsigned>(0));
    TS_ASSERT_EQUALS(static_cast<int>(ie.address()), 1);
    TS_ASSERT_THROWS_NOTHING(ie.next());

    TS_ASSERT_EQUALS(ie.cycle(), static_cast<unsigned>(1));
    TS_ASSERT_EQUALS(static_cast<int>(ie.address()), 1);
    TS_ASSERT_THROWS_NOTHING(ie.next());

    TS_ASSERT_EQUALS(ie.cycle(), static_cast<unsigned>(2));
    TS_ASSERT_EQUALS(static_cast<int>(ie.address()), 3);
    TS_ASSERT_THROWS_NOTHING(ie.next());

    TS_ASSERT_EQUALS(ie.cycle(), static_cast<unsigned>(3));
    TS_ASSERT_EQUALS(static_cast<int>(ie.address()), 14);

    TS_ASSERT_EQUALS(ie.hasNext(), false);  
    TS_ASSERT_THROWS(ie.next(), NotAvailable);

    delete execTrace_;
    execTrace_ = NULL;
}


#endif
