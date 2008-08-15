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
