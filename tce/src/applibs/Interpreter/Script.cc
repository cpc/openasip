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
