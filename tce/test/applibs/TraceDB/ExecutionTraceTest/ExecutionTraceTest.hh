/** 
 * @file ExecutionTraceTest.hh 
 *
 * A test suite for TraceDB.
 * 
 * @author Pekka J‰‰skel‰inen 2004 (pjaaskel@cs.tut.fi)
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
