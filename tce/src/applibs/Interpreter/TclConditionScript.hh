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
