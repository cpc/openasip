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
 * @file Script.hh
 *
 * Declaration of Script class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_SCRIPT_HH
#define TTA_SCRIPT_HH

#include <string>
#include <vector>

#include "ScriptInterpreter.hh"
#include "DataObject.hh"
#include "Exception.hh"

/**
 * Helper class that contains line(s) of script that can be executed on
 * demand.
 */
class Script {
public:
    Script(ScriptInterpreter* interpreter, std::string scriptLine);
    Script(ScriptInterpreter* interpreter, std::vector<std::string>& script);
    virtual ~Script();

    virtual DataObject execute();
    DataObject lastResult();
    virtual std::vector<std::string> script() const;
protected:
    /// Interpreter executing the commands.
    ScriptInterpreter* interpreter_;
private:
    /// Result of execution.
    DataObject result_;
    /// Flag indicating whether script has been executed or not.
    bool executeCalled_;
    /// Contains script.
    std::vector<std::string> scriptLines_;
};

#include "Script.icc"

#endif
