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
 * @file SimpleScriptInterpreterTest.hh.
 *
 * A test suite for SimpleScriptInterpreter.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_SCRIPT_INTERPRETER_TEST_HH
#define TTA_SIMPLE_SCRIPT_INTERPRETER_TEST_HH

#include <TestSuite.h>
#include <string>
#include <vector>

#include "SimpleScriptInterpreter.hh"
#include "Conversion.hh"
#include "CustomCommand.hh"
#include "DataObject.hh"

using std::string;
using std::vector;

//////////////////////////////////////////////////////////////////////////////
// MulCmd
//////////////////////////////////////////////////////////////////////////////
/**
 * Test custom command that calculates the multiplication of its
 * parameters.
 */
class MulCmd : public CustomCommand {
public:
    MulCmd();
    virtual ~MulCmd();

    virtual bool execute(const vector<DataObject>& arguments);
    virtual string helpText() const;
};

/**
 * Constructor.
 */
MulCmd::MulCmd() : CustomCommand("mul") {
}

/**
 * Destructor.
 */
MulCmd::~MulCmd() {
}

/**
 * Executest the mul command.
 *
 * @param arguments Arguments for the command.
 * @return True if execution is successful, false otherwise.
 * @exception NumberFormatException If data object conversion fails.
 */
bool
MulCmd::execute(const vector<DataObject>& arguments) {
    ScriptInterpreter* interp = interpreter();
    DataObject* result = new DataObject();
    if (arguments.size() != 3) {
        string number = Conversion::toString(arguments.size());
        result->setString("Wrong number of arguments: " + number);
        interp->setResult(result);
        return false;
    }

    int arg1, arg2;
    try {
        arg1 = arguments[1].integerValue();
        arg2 = arguments[2].integerValue();
    } catch (const NumberFormatException& n) {
        delete result;
        throw n;
    }
    result->setInteger(arg1 * arg2);
    interp->setResult(result);
    return true;
}

/**
 * Returns the help text of the command.
 *
 * @return Help text of the command.
 */
string
MulCmd::helpText() const {
    return "mul <param1> <param2>";
}

//////////////////////////////////////////////////////////////////////////////
// SimpleScriptInterpreterTest
//////////////////////////////////////////////////////////////////////////////

/**
 * Test class for SimpleScriptInterpreter.
 */
class SimpleScriptInterpreterTest : public CxxTest::TestSuite {
public:
    void setUp() {}
    void tearDown() {}

    void testInterpret();
    void testVariables();
};

/**
 * Test that interpreting works.
 */
void
SimpleScriptInterpreterTest::testInterpret() {

    SimpleScriptInterpreter interpreter;
    const char* arg0 = "./foo";
    char* argv[] = {const_cast<char*>(arg0)};
    interpreter.initialize(1, argv, NULL, NULL);
    
    MulCmd* cmd = new MulCmd();
    interpreter.addCustomCommand(cmd);
    
    TS_ASSERT_EQUALS(interpreter.interpret("mul 3 3"), true);
    TS_ASSERT_EQUALS(interpreter.result(), "9");

    TS_ASSERT_EQUALS(interpreter.interpret("mul 2"), false);

    TS_ASSERT_EQUALS(interpreter.interpret("foo bar"), false);
    TS_ASSERT_EQUALS(interpreter.result(), "Unknown command: foo"); 

    TS_ASSERT_EQUALS(interpreter.interpret("mul foo 2"), false);
    TS_ASSERT_EQUALS(interpreter.interpret(""), true);

    interpreter.finalize();
}

/**
 * Tests that setting variables works.
 */
void
SimpleScriptInterpreterTest::testVariables() {
    
    string stringVariable = "foo";
    string intVariable = "bar";

    SimpleScriptInterpreter interpreter;
    const char* arg0 = "./foo";
    char* argv[] = {const_cast<char*>(arg0)};
    interpreter.initialize(1, argv, NULL, NULL);

    interpreter.setVariable("foo", "bar");
    interpreter.setVariable("bar", 20);
    
    TS_ASSERT_EQUALS(interpreter.variableStringValue("foo"), "bar");
    TS_ASSERT_EQUALS(interpreter.variableIntegerValue("bar"), 20);

    interpreter.setVariable("foo", "daa");

    TS_ASSERT_EQUALS(interpreter.variableStringValue("foo"), "daa");
    
    interpreter.finalize();
}

#endif
