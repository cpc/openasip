/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file Script.cc
 *
 * Definition of Script class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
Script::execute() {
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
