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
 * @file ExecutionTraceTest.hh 
 *
 * A test suite for TraceDB.
 * 
 * @author Pekka J‰‰skel‰inen 2004 (pjaaskel-no.spam-cs.tut.fi)
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
