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
 * @file ConditionScript.cc
 *
 * Definition of ConditionScript class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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

