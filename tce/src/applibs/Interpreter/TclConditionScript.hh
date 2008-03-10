/**
 * @file TclConditionScript.hh
 *
 * Declaration of TclConditionScript class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#ifndef TTA_TCL_CONDITION_SCRIPT_HH
#define TTA_TCL_CONDITION_SCRIPT_HH

#include <string>
#include <vector>

#include "Exception.hh"
#include "ConditionScript.hh"

class TclInterpreter;

/**
 * Class that contains a script that defines a condition for some action.
 *
 * A version tweaked to work with Tcl. Tcl does not allow condition to be 
 * an expression with a value, so I had to wrap the condition in a 
 * an if ... else .. expression to make it work.
 */
class TclConditionScript : public ConditionScript {
public:
    TclConditionScript(
        TclInterpreter* interpreter, std::string scriptLine);
    virtual ~TclConditionScript();

    virtual std::vector<std::string> script() const;

    virtual ConditionScript* copy() const;
private:
    std::string displayedCondition_;
};

#endif
