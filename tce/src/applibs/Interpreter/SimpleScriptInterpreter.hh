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
 * @file SimpleScriptInterpreter.hh
 *
 * Declaration of SimpleScriptInterpreter.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
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
        const std::string& name, const DataObject& value);

    virtual DataObject variable(const std::string& name);
    virtual bool interpret(const std::string& commandLine);
    virtual void setResultToInterpreter(const DataObject& value);

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
