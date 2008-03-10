/** 
 * @file OperationPoolTest.hh
 * 
 * A test suite for OperationPool.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen@tut.fi)
 */

#ifndef OPERATION_POOL_TEST_HH
#define OPERATION_POOL_TEST_HH

#include <TestSuite.h>
#include <vector>

#include "SimValue.hh"
#include "OperationPool.hh"
#include "Operation.hh"
#include "OperationContext.hh"
#include "StringTools.hh"

using std::vector;

/**
 * This class is used to test OperationPool.
 */
class OperationPoolTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testOperation();
    void testDAGOperation();

private:
};


/**
 * Called before each test.
 */
void
OperationPoolTest::setUp() {
}


/**
 * Called after each test.
 */
void
OperationPoolTest::tearDown() {
}

/**
 * Test that operation() method works.
 */
void
OperationPoolTest::testOperation() {
    
    OperationPool pool;

    Operation& call = pool.operation("call");
    TS_ASSERT(StringTools::ciEqual(call.name(), "call"));
    TS_ASSERT(call.isControlFlowOperation());

    Operation& jump = pool.operation("jump");
    TS_ASSERT(StringTools::ciEqual(jump.name(), "jump"));
    TS_ASSERT(jump.isControlFlowOperation());

    Operation& oper = pool.operation("oper1");
    TS_ASSERT_DIFFERS(&oper, &NullOperation::instance());

    Operation& oper2 = pool.operation("foobariehbfa");
    TS_ASSERT_EQUALS(&oper2, &NullOperation::instance());
    TS_ASSERT_EQUALS(oper.numberOfInputs(), 1);
    TS_ASSERT_EQUALS(oper.numberOfOutputs(), 1);

    Operation& oper3 = pool.operation("oper3");
    TS_ASSERT_EQUALS(oper3.dagCount(), 2);

    OperationContext context;
    
    SimValue** args = new SimValue*[2];
    SimValue input1(32);
    input1 = 1;
    args[0] = &input1;

    SimValue result (32);
    args[1] = &result;

    TS_ASSERT_EQUALS(oper.simulateTrigger(args, context), true);
    TS_ASSERT_EQUALS(result.intValue(), 1 * 3);

    OperationPool::cleanupCache();

    delete[] args;
    args = NULL;
}

/**
 * Test that operation() method works.
 */
void
OperationPoolTest::testDAGOperation() {
    
    OperationPool pool;
    
    Operation& addsub = pool.operation("addsub");
    TS_ASSERT(StringTools::ciEqual(addsub.name(), "addsub"));
    TS_ASSERT_EQUALS(addsub.numberOfInputs(), 2);
    TS_ASSERT_EQUALS(addsub.numberOfOutputs(), 2);
    
    SimValue a,b,c,d;
    a = 13; b = 14; c = 33; d = 44;
    SimValue* params[] = {&a, &b, &c, &d};    
    SimValue** paramsValid = params;    
    OperationContext context;
    addsub.simulateTrigger(paramsValid, context);
    
    TS_ASSERT_EQUALS(c.intValue(), 13 + 14);
    TS_ASSERT_EQUALS(d.intValue(), 13 - 14);
    OperationPool::cleanupCache();
}

#endif
