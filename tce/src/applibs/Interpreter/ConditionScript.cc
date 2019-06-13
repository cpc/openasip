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
ConditionScript::conditionOk() {
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

