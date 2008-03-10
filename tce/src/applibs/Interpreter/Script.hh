/**
 * @file Script.hh
 *
 * Declaration of Script class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#ifndef TTA_SCRIPT_HH
#define TTA_SCRIPT_HH

#include <string>
#include <vector>

#include "ScriptInterpreter.hh"
#include "DataObject.hh"
#include "Exception.hh"

/**
 * Helper class that contains line(s) of script that can be executed on
 * demand.
 */
class Script {
public:
    Script(ScriptInterpreter* interpreter, std::string scriptLine);
    Script(ScriptInterpreter* interpreter, std::vector<std::string>& script);
    virtual ~Script();

    virtual DataObject execute() 
        throw (ScriptExecutionFailure, NumberFormatException);
    DataObject lastResult() throw (InvalidData);
    virtual std::vector<std::string> script() const;
protected:
    /// Interpreter executing the commands.
    ScriptInterpreter* interpreter_;
private:
    /// Result of execution.
    DataObject result_;
    /// Flag indicating whether script has been executed or not.
    bool executeCalled_;
    /// Contains script.
    std::vector<std::string> scriptLines_;
};

#include "Script.icc"

#endif
