/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file TclConditionScript.cc
 *
 * Definition of TclConditionScript class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
