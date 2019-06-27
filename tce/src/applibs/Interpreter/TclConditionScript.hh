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
 * @file TclConditionScript.hh
 *
 * Declaration of TclConditionScript class.
 *
 * @author Pekka J��skel�inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
