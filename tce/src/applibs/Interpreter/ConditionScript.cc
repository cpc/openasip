/**
 * @file ConditionScript.cc
 *
 * Definition of ConditionScript class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "ConditionScript.hh"

/**
 * Constructor.
 *
 * @param interpreter Interpreter for the script.
 * @param scriptLine A line of script.
 */
ConditionScript::ConditionScript(
    ScriptInterpreter* interpreter, 
    std::string scriptLine) : Script(interpreter, scriptLine) {
}

/**
 * Constructor.
 *
 * @param interpreter Interpreter for the script.
 * @param script The script.
 */
ConditionScript::ConditionScript(
    ScriptInterpreter* interpreter,
    std::vector<std::string>& script) : Script(interpreter, script) {
}

/**
 * Destructor.
 */
ConditionScript::~ConditionScript() {
}

/**
 * Tests if condition is true of false.
 *
 * @return True, if condition is true, false otherwise.
 * @exception NumberFormatException If DataObject operation fails.
 * @exception ScriptExecutionFailure If script execution fails.
 */
bool
ConditionScript::conditionOk() 
    throw (NumberFormatException, ScriptExecutionFailure) {
    
    vector<string> scripts = script();
    if (scripts.size() == 1 && scripts[0] == "") {
        return true;
    }

    DataObject obj = execute();
    if (obj.stringValue() == "0") {
        interpreter_->setResult("");
        return false;
    }
    interpreter_->setResult("");
    return true;
}

/**
 * Copies the condition script.
 *
 * Allows dynamically bound copy.
 *
 * @return a new instance which is identical to this.
 */
ConditionScript* 
ConditionScript::copy() const {
    return new ConditionScript(*this);
}

