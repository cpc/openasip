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
 * @file OperationPoolTest.hh
 * 
 * A test suite for OperationPool.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
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
#include "TCEString.hh"

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
