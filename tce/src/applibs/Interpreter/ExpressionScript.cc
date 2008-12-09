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
 * @file ExpressionScript.cc
 *
 * Definition of ExpressionScript class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
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
