/**
 * @file ConditionScript.hh
 *
 * Declaration of ConditionScript class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#ifndef TTA_CONDITION_SCRIPT_HH
#define TTA_CONDITION_SCRIPT_HH

#include <string>
#include <vector>

#include "Script.hh"
#include "Exception.hh"

/**
 * Class that contains a script that defines a condition for some action.
 */
class ConditionScript : public Script {
public:
    ConditionScript(ScriptInterpreter* interpreter, std::string scriptLine);
    ConditionScript(
        ScriptInterpreter* interpreter, 
        std::vector<std::string>& script);
    virtual ~ConditionScript();

    virtual bool conditionOk() 
        throw (NumberFormatException, ScriptExecutionFailure);

    virtual ConditionScript* copy() const;
};

#endif
