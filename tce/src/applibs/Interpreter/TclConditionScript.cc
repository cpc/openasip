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
