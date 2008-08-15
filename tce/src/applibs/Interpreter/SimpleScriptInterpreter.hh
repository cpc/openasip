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
 * @file SimpleScriptInterpreter.hh
 *
 * Declaration of SimpleScriptInterpreter.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_SCRIPT_INTERPRETER_HH
#define TTA_SIMPLE_SCRIPT_INTERPRETER_HH

#include <map>
#include <string>

#include "ScriptInterpreter.hh"

class InterpreterContext;
class LineReader;

/**
 * Interpreter which uses only custom commands.
 *
 * Custom commands are user defined commands.
 */
class SimpleScriptInterpreter : public ScriptInterpreter {
public:
    SimpleScriptInterpreter();
    virtual ~SimpleScriptInterpreter();

    virtual void initialize(
        int argc, 
        char* argv[], 
        InterpreterContext* context, 
        LineReader* reader);

    virtual void setVariableToInterpreter(
        const std::string& name,
        const DataObject& value
        ) throw (NumberFormatException);

    virtual DataObject variable(const std::string& name);
    virtual bool interpret(const std::string& commandLine);
    virtual void setResultToInterpreter(const DataObject& value)
        throw (NumberFormatException);

    virtual InterpreterContext& context() const;

protected:
    virtual void addCustomCommandToInterpreter(const CustomCommand& command);
    virtual void removeCustomCommandFromInterpreter(
        const CustomCommand& command);
private:
    /// Map for variables.
    typedef std::map<std::string, std::string> VariableMap;

    /// Copying not allowed.
    SimpleScriptInterpreter(const SimpleScriptInterpreter&);
    /// Assignment not allowed.
    SimpleScriptInterpreter& operator=(const ScriptInterpreter&);

    /// Holds all the variables given to interpreter.
    VariableMap variables_;
    /// Context for interpreter.
    InterpreterContext* context_;
};

#endif
