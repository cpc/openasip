/**
 * @file TclConditionScript.cc
 *
 * Definition of TclConditionScript class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 */

#include <string>
using std::string;
#include <vector>
using std::vector;

#include "TclConditionScript.hh"
#include "TclInterpreter.hh"

/**
 * Constructor.
 *
 * The script is converted to an if-statement and the original condition
 * is saved for displaying purposes.
 *
 * @param interpreter Interpreter for the script.
 * @param scriptLine A line of script.
 */
TclConditionScript::TclConditionScript(
    TclInterpreter* interpreter, 
    std::string scriptLine) :
    ConditionScript(
        interpreter, std::string("if {") + scriptLine + 
        "} { set _last_condition_value_ 1 } else "
        "{ set _last_condition_value_ 0 }"), displayedCondition_(scriptLine) {
}

/**
 * Destructor.
 */
TclConditionScript::~TclConditionScript() {
}

/**
 * Returns the script.
 *
 * @return The script.
 */
std::vector<std::string>
TclConditionScript::script() const {
    std::vector<std::string> container;
    container.push_back(displayedCondition_);
    return container;
}

/**
 * Copies the condition script.
 *
 * Allows dynamically bound copy.
 *
 * @return a new instance which is identical to this.
 */
ConditionScript* 
TclConditionScript::copy() const {
    return new TclConditionScript(*this);
}
