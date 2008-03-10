/**
 * @file SimpleScriptInterpreterTest.hh.
 *
 * A test suite for SimpleScriptInterpreter.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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

    virtual bool execute(const vector<DataObject>& arguments)
        throw (NumberFormatException);
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
MulCmd::execute(const vector<DataObject>& arguments) 
    throw (NumberFormatException) {

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
