/**
 * @file ExpressionScript.hh
 *
 * Declaration of ExpressionScript class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 */

#ifndef TTA_EXPRESSION_SCRIPT_HH
#define TTA_EXPRESSION_SCRIPT_HH

#include <string>
#include <vector>

#include "Script.hh"
#include "Exception.hh"

/**
 * ExpressionScript contains a script which changes can be inspected.
 */
class ExpressionScript : public Script {
public:
    ExpressionScript(ScriptInterpreter* interpreter, std::string scriptLine);
    ExpressionScript(
        ScriptInterpreter* interpreter, 
        std::vector<std::string>& script);
    virtual ~ExpressionScript();

    bool resultChanged() 
        throw (ScriptExecutionFailure, NumberFormatException);
};

#endif
