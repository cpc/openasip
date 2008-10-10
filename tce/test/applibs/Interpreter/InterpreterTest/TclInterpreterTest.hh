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
 * @file TclInterpreterTest.hh 
 *
 * A test suite for TclInterpreter.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 27 May 2004 by pj, jn, vpj, ll
 * @note rating: yellow
 */

#ifndef TCL_INTERPRETER_TEST_HH
#define TCL_INTERPRETER_TEST_HH

#include <TestSuite.h>
#include <string>
#include <vector>

#include "TclInterpreter.hh"
#include "DataObject.hh"
#include "CmdHelp.hh"
#include "CustomCommand.hh"
#include "Conversion.hh"
#include "Exception.hh"
#include "FileSystem.hh"

using std::string;
using std::vector;

/**
 * Test CustomCommand.
 *
 * Calculates the sum of its two parameters.
 */
class ArithCmd : public CustomCommand {
public:
    ArithCmd();
    virtual ~ArithCmd();

    virtual bool execute(const vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual string helpText() const;
};

/**
 * Constructor.
 */
ArithCmd:: ArithCmd() : CustomCommand("add") {
}

/**
 * Destructor.
 */
ArithCmd::~ArithCmd() {
}

/**
 * Executes the add command.
 *
 * @param arguments Arguments for the command.
 * @return True, if execution was successful, false otherwise.
 * @exception NumberFormatException If conversion of DataObject fails. 
 */
bool
ArithCmd::execute(const vector<DataObject>& arguments) 
    throw (NumberFormatException) {
    
    ScriptInterpreter* interp = interpreter();

    DataObject* obj = new DataObject();
    if (arguments.size() != 3) {
        string nmbr = Conversion::toString(arguments.size());
        obj->setString("Error: Wrong number of arguments: " + nmbr);
        interp->setResult(obj);
        return false;
    }
    
    int arg1 = arguments[1].integerValue();
    int arg2 = arguments[2].integerValue();
    obj->setInteger(arg1 + arg2);
    interp->setResult(obj);
    return true;
}

/**
 * Returns the help text for the command.
 *
 * @return The help text.
 */
string
ArithCmd::helpText() const {
    return "add <param1> <param2>";
}

/**
 * Test CustomCommand that returns a list.
 */
class ListCmd : public CustomCommand {
public:
    ListCmd();
    virtual ~ListCmd();

    virtual bool execute(const vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual string helpText() const;
};

/**
 * Constructor.
 */
ListCmd::ListCmd() : CustomCommand("slist") {
}

/**
 * Destructor.
 */
ListCmd::~ListCmd() {
}

/**
 * Executes the slist command.
 *
 * @param arguments Arguments for the command.
 * @return True.
 * @exception NumberFormatException If conversion of DataObject fails.
 */
bool
ListCmd::execute(const vector<DataObject>& arguments) 
    throw (NumberFormatException) {

    ScriptInterpreter* interp = interpreter();

    string result = "";
    for (unsigned int i = 1; i < arguments.size(); i++) {
        result += arguments[i].stringValue() + " ";
    }
    DataObject* obj = new DataObject();
    obj->setString(result);
    interp->setResult(obj);
    return true;
}

/**
 * Returns the help text.
 *
 * @return The help text.
 */
string
ListCmd::helpText() const {
    return "slist <param> ...";
}

/**
 * Test class for TclInterpreter.
 */
class TclInterpreterTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testInterpret();
    void testAddVariable();
    void testError();
    void testHelpCommand();
    void testProcessScriptFile();
    void testArgcArgv();
    void testCustomCommands();
    void testWhileForLoops();

private:
    /// Name of the integer variable.
    static const string INT_VAR_NAME_;
    /// Name of the string variable.
    static const string STRING_VAR_NAME_;
    /// Name of the script file.
    static const string SCRIPT_FILE_;
};

const string TclInterpreterTest::INT_VAR_NAME_ = "intVar";
const string TclInterpreterTest::STRING_VAR_NAME_ = "stringVar";
const string TclInterpreterTest::SCRIPT_FILE_ = "data" +
FileSystem::DIRECTORY_SEPARATOR + "script";

/**
 * Called before each test.
 */
void
TclInterpreterTest::setUp() {
}


/**
 * Called after each test.
 */
void
TclInterpreterTest::tearDown() {
}

/**
 * Tests that interpreting works.
 */
void
TclInterpreterTest::testInterpret() {
    TclInterpreter interpreter;
    InterpreterContext* context = new InterpreterContext();
    const char* arg0 = "./foo";
    char* argv[] = {const_cast<char*>(arg0)};
    interpreter.initialize(1, argv, context, NULL);
    TS_ASSERT_EQUALS(interpreter.interpret("set foo 3"), true);
    TS_ASSERT_EQUALS(interpreter.result(), "3");
    TS_ASSERT_EQUALS(&interpreter.context(), context);
    interpreter.finalize();
    delete context;
}

/**
 * Tests that variables can be added to interpreter.
 */
void
TclInterpreterTest::testAddVariable() {
    TclInterpreter interpreter;
    const char* arg0 = "./foo";
    char* argv[] = {const_cast<char*>(arg0)};
    interpreter.initialize(1, argv, NULL, NULL);
    interpreter.setVariable(INT_VAR_NAME_, 10);
    interpreter.setVariable(STRING_VAR_NAME_, string("foo"));
    TS_ASSERT_EQUALS(interpreter.variableIntegerValue(INT_VAR_NAME_), 10);
    TS_ASSERT_EQUALS(interpreter.variableStringValue(STRING_VAR_NAME_), "foo");
    DataObject var = interpreter.variable(INT_VAR_NAME_);
    TS_ASSERT_EQUALS(var.integerValue(), 10);
    interpreter.finalize();
}

/**
 * Tests that interpreter is set to error state, when invalid command is
 * given to it.
 */
void
TclInterpreterTest::testError() {
    TclInterpreter interpreter;
    const char* arg0 = "./foo";
    char* argv[] = {const_cast<char*>(arg0)};
    interpreter.initialize(1, argv, NULL, NULL);
    TS_ASSERT_EQUALS(interpreter.interpret("foo"), false);
    TS_ASSERT_EQUALS(interpreter.error(), true);
    interpreter.finalize();
}

/**
 * Tests that help command works.
 */
void
TclInterpreterTest::testHelpCommand() {
    TclInterpreter interpreter;
    const char* arg0 = "./foo";
    char* argv[] = {const_cast<char*>(arg0)};
    interpreter.initialize(1, argv, NULL, NULL);
    CmdHelp* hlpCmd = new CmdHelp();
    interpreter.addCustomCommand(hlpCmd);
    TS_ASSERT_EQUALS(interpreter.interpret("help"), true);
    TS_ASSERT_EQUALS(interpreter.interpret("help foo"), false);
    TS_ASSERT_EQUALS(interpreter.interpret("help help"), true);
    TS_ASSERT_EQUALS(interpreter.result(), "Use help command to get help");
    interpreter.finalize();
}

/**
 * Tests that interpreter processes script file correctly.
 */
void
TclInterpreterTest::testProcessScriptFile() {
    TclInterpreter interpreter;
    const char* arg0 = "./foo";
    char* argv[] = {const_cast<char*>(arg0)};
    interpreter.initialize(1, argv, NULL, NULL);
    TS_ASSERT_EQUALS(interpreter.processScriptFile(SCRIPT_FILE_), true);
    TS_ASSERT_EQUALS(interpreter.variableIntegerValue("foo"), 3);
    TS_ASSERT_EQUALS(interpreter.interpret("truth"), true);
    TS_ASSERT_EQUALS(interpreter.result(), "Jussi rules");
    TS_ASSERT_EQUALS(interpreter.interpret("calc 3"), true);
    TS_ASSERT_EQUALS(interpreter.result(), "9");
    interpreter.finalize();
}

/**
 * Tests that argc and argv parameters are initialized correctly to
 * interpreter.
 */
void
TclInterpreterTest::testArgcArgv() {
    TclInterpreter interpreter;
    const char* arg0 = "./foo";
    const char* arg1 = "bar";
    const char* arg2 = "goo";
    char* argC0 = const_cast<char*>(arg0);
    char* argC1 = const_cast<char*>(arg1);
    char* argC2 = const_cast<char*>(arg2);
    char* argv[] = {argC0, argC1, argC2};
    interpreter.initialize(3, argv, NULL, NULL);
    TS_ASSERT_EQUALS(interpreter.variableIntegerValue("argc"), 2);
    TS_ASSERT_EQUALS(interpreter.variableStringValue("argv0"), "./foo");
    TS_ASSERT_EQUALS(interpreter.variableStringValue("argv"), "bar goo");
    interpreter.finalize();
}

/**
 * Tests that CustomCommand works well with tcl commands.
 */
void
TclInterpreterTest::testCustomCommands() {
    TclInterpreter interpreter;
    const char* arg0 = "./foo";
    char* argv[] = {const_cast<char*>(arg0)};
    interpreter.initialize(1, argv, NULL, NULL);
    ArithCmd* acmd = new ArithCmd();
    ListCmd* lcmd = new ListCmd();
    interpreter.addCustomCommand(acmd);
    interpreter.addCustomCommand(lcmd);
    
    CustomCommand* arith = interpreter.customCommand("add");
    TS_ASSERT_EQUALS(acmd, arith);
    
	vector<string> names = interpreter.customCommandsSortedByName();
	TS_ASSERT_EQUALS(static_cast<int>(names.size()), 2);
	TS_ASSERT_EQUALS(names[0], "add");
	TS_ASSERT_EQUALS(names[1], "slist")

    TS_ASSERT_EQUALS(interpreter.interpret("add 2 2"), true);
    TS_ASSERT_EQUALS(interpreter.result(), "4");
    TS_ASSERT_EQUALS(interpreter.interpret("expr 4 * [add 2 2]"), true);
    TS_ASSERT_EQUALS(interpreter.result(), "16");
    TS_ASSERT_EQUALS(interpreter.interpret("expr 4 * [add 2]"), false);
    string command = "foreach {i} [slist a b c] {}";
    TS_ASSERT_EQUALS(interpreter.interpret(command), true);

    // test removing custom command
    interpreter.removeCustomCommand("add");
    interpreter.removeCustomCommand("slist");
    TS_ASSERT_EQUALS(interpreter.interpret("add 2 2"), false);
    TS_ASSERT_EQUALS(interpreter.interpret("slist a b c"), false);
    interpreter.finalize();
}

/**
 * Test that while and for loops works.
 */
void
TclInterpreterTest::testWhileForLoops() {
    TclInterpreter interpreter;
    const char* arg0 = "./foo";
    char* argv[] = {const_cast<char*>(arg0)};
    interpreter.initialize(1, argv, NULL, NULL);
    string whileCmd = "set i 0; while {$i < 10} { incr i }";
    TS_ASSERT_EQUALS(interpreter.interpret(whileCmd), true);
    int value = 0;
    TS_ASSERT_THROWS_NOTHING(value = interpreter.variableIntegerValue("i"));
    TS_ASSERT_EQUALS(value, 10);
    string forCmd = "set a 2; for {set i 1} {$i <= 4} \
         {incr i} { set a [expr $a * $a] }";
    TS_ASSERT_EQUALS(interpreter.interpret(forCmd), true);
    TS_ASSERT_THROWS_NOTHING(value = interpreter.variableIntegerValue("a"));
    TS_ASSERT_EQUALS(value, 65536);
    interpreter.finalize();
}

#endif
