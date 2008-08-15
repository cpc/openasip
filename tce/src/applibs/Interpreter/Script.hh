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
 * @file Script.hh
 *
 * Declaration of Script class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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

    virtual DataObject execute() 
        throw (ScriptExecutionFailure, NumberFormatException);
    DataObject lastResult() throw (InvalidData);
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
