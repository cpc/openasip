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
 * @file ScriptTest.hh 
 *
 * A test suite for Script.
 * 
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 */

#ifndef TTA_SCRIPT_TEST_HH
#define TTA_SCRIPT_TEST_HH

#include <TestSuite.h>
#include <vector>
#include <string>

#include "ExpressionScript.hh"
#include "ConditionScript.hh"
#include "TclInterpreter.hh"

using std::vector;
using std::string;

/**
 * Class that tests ExpressionScript and ConditionScript.
 */
class ScriptTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testExpressionScript();
    void testConditionScript();
    void testError();
    
private:
};


/**
 * Called before each test.
 */
void
ScriptTest::setUp() {
}


/**
 * Called after each test.
 */
void
ScriptTest::tearDown() {
}

/**
 * Tests that ExpressionScript works.
 */
void
ScriptTest::testExpressionScript() {
    
    TclInterpreter interpreter;
    const char* arg0 = "./foo";
    char* argv[] = {const_cast<char*>(arg0)};
    interpreter.initialize(1, argv, NULL, NULL);
    ExpressionScript expression1(&interpreter, "expr 3 * 3");
    TS_ASSERT_THROWS_NOTHING(expression1.execute());
    TS_ASSERT_EQUALS(expression1.resultChanged(), false);
    
    vector<string> scripts;
    scripts.push_back("expr 3 * 3");
    scripts.push_back("expr 4 * 4");

    ExpressionScript expression2(&interpreter, scripts);
    DataObject result;
    TS_ASSERT_THROWS_NOTHING(result = expression2.execute());
    TS_ASSERT_EQUALS(result.integerValue(), 16);
    interpreter.finalize();
}

/**
 * Tests that ConditionScript works.
 */
void
ScriptTest::testConditionScript() {

    TclInterpreter interpreter;
    const char* arg0 = "./foo";
    char* argv[] = {const_cast<char*>(arg0)};
    interpreter.initialize(1, argv, NULL, NULL);
    ConditionScript trueCond(&interpreter, "expr 3 > 2");
    ConditionScript falseCond(&interpreter, "expr 2 > 3");
    ConditionScript emptyCond(&interpreter, "");
    TS_ASSERT_EQUALS(trueCond.conditionOk(), true);
    TS_ASSERT_EQUALS(falseCond.conditionOk(), false);
    TS_ASSERT_EQUALS(emptyCond.conditionOk(), true);
    interpreter.finalize();
}

/**
 * Test that error is indicated by throwing an exception.
 */
void
ScriptTest::testError() {
    TclInterpreter interpreter;
    const char* arg0 = "./foo";
    char* argv[] = {const_cast<char*>(arg0)};
    interpreter.initialize(1, argv, NULL, NULL);
    ExpressionScript errorExp(&interpreter, "expr foo");
    ExpressionScript expr(&interpreter, "expr 3 > 2");
    TS_ASSERT_EQUALS(expr.resultChanged(), false);
    TS_ASSERT_THROWS(errorExp.execute(), ScriptExecutionFailure);
    interpreter.finalize();
}

#endif
