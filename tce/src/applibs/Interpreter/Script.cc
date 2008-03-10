/**
 * @file Script.cc
 *
 * Definition of Script class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "Script.hh"
#include "Application.hh"

/**
 * Constructor.
 *
 * @param interpreter Interpreter for script.
 * @param scriptLine A line of script.
 */
Script::Script(ScriptInterpreter* interpreter, std::string scriptLine) : 
    interpreter_(interpreter), executeCalled_(false) {
    scriptLines_.push_back(scriptLine);
}

/**
 * Constructor.
 *
 * @param interpreter Interpreter for script.
 * @param script Script lines.
 */
Script::Script(
    ScriptInterpreter* interpreter, 
    std::vector<std::string>& script) {
    
    interpreter_ = interpreter;
    for (unsigned int i = 0; i < script.size(); i++) {
        scriptLines_.push_back(script[i]);
    }
}

/**
 * Destructor.
 */
Script::~Script() {
}

/**
 * Executes the script.
 *
 * @return The result of execution as a DataObject.
 * @exception ScriptExecutionFailure If execution of script fails.
 * @exception NumberFormatException If DataObject operation fails.
 */
DataObject
Script::execute() 
    throw (ScriptExecutionFailure, NumberFormatException) {

    Application::initialize();
    
    for (unsigned int i = 0; i < scriptLines_.size(); i++) {
        if (!interpreter_->interpret(scriptLines_[i])) {
            string method = "Script::execute()";
            string message = "Interpreter error: " + 
                interpreter_->result();
            throw ScriptExecutionFailure(__FILE__, __LINE__, method, message);
        }
    }
    DataObject obj;
    obj.setString(interpreter_->result());
    result_ = obj;
    executeCalled_ = true;
    return obj;
}

/**
 * Returns the script.
 *
 * @return The script.
 */
std::vector<std::string>
Script::script() const {
    return scriptLines_;
}
