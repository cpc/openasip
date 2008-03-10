/**
 * @file ExpressionScript.cc
 *
 * Definition of ExpressionScript class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 */

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "ExpressionScript.hh"

/**
 * Constructor.
 *
 * @param interpreter The interpreter for the script.
 * @param scriptLine A line of script.
 */
ExpressionScript::ExpressionScript(
    ScriptInterpreter* interpreter, 
    std::string scriptLine) : Script(interpreter, scriptLine) {
}

/**
 * Constructor.
 *
 * @param interpreter The interpreter for the script.
 * @param script The script.
 */
ExpressionScript::ExpressionScript(
    ScriptInterpreter* interpreter,
    std::vector<std::string>& script) : Script(interpreter, script) {
}

/**
 * Destructor.
 */
ExpressionScript::~ExpressionScript() {
}

/**
 * Returns true, if the result of the script has changed.
 *
 * @return True, if the result of the script has changed, otherwise false.
 * @exception ScriptExecutionFailure If the execution of the script fails.
 * @exception NumberFormatException If DataObject operation fails.
 */
bool
ExpressionScript::resultChanged() 
    throw (ScriptExecutionFailure, NumberFormatException) {

    DataObject obj;
    try {
        obj = lastResult();
    } catch (const InvalidData& i) {
        return false;
    }

    DataObject exec = execute();

    interpreter_->setResult("");

    if (obj != exec) {
        return true;
    }
    return false;
}
