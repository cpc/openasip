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
 * @file ScriptInterpreter.hh
 *
 * The declaration of ScriptInterpreter class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 27 May 2004 by pj, jn, vpj, ll
 * @note rating: yellow
 */

#ifndef TTA_SCRIPT_INTERPRETER_HH
#define TTA_SCRIPT_INTERPRETER_HH

#include <map>
#include <string>
#include <vector>

#include "CustomCommand.hh"
#include "DataObject.hh"
#include "Exception.hh"
#include "LineReader.hh"

class CustomCommand;

/**
 * Abstract base class for all interpreters.
 *
 * Interpreter processes its input, takes actions according to it and returns
 * to wait more input.
 */
class ScriptInterpreter {
public:
    ScriptInterpreter();
    virtual ~ScriptInterpreter();

    virtual void addCustomCommand(CustomCommand* command);
    virtual void removeCustomCommand(const std::string& commandName);

    virtual CustomCommand* customCommand(const std::string& commandName);
    virtual void setResult(DataObject* result);
    virtual void setResult(const std::string& result);
    virtual void setResult(int result);
    virtual void setResult(double result);
    virtual std::string result();

    virtual void setError(bool state);
    virtual void setError(std::string errorMessage);
    virtual bool error() const;

    virtual void setVariable(
        const std::string& interpreterVariableName, 
        const std::string& value);
    virtual void setVariable(
        const std::string& interpreterVariableName, 
        int value);

    virtual std::string variableStringValue(
        const std::string& interpreterVariableName);
    virtual int variableIntegerValue(
        const std::string& interpreterVariableName);

    virtual bool processScriptFile(const std::string& scriptFileName);

    virtual void finalize();

    virtual void setLineReader(LineReader* reader);
    virtual LineReader* lineReader() const;

	std::vector<std::string> customCommandsSortedByName();

    virtual void initialize(
        int argc, 
        char* argv[], 
        InterpreterContext* context,
        LineReader* reader) = 0;

    virtual void setVariableToInterpreter(
        const std::string& name, const DataObject& value) = 0;
    virtual DataObject variable(const std::string& name) = 0;
    virtual bool interpret(const std::string& commandLine) = 0;
    virtual void setResultToInterpreter(const DataObject& value) = 0;

    virtual InterpreterContext& context() const = 0;
   
protected:
    virtual void addCustomCommandToInterpreter(
        const CustomCommand& command) = 0;
    virtual void removeCustomCommandFromInterpreter(
        const CustomCommand& command) = 0;

private:
    /// Iterator for map.
    typedef std::map<std::string, CustomCommand*>::iterator MapIter;
    /// val_type for map.
    typedef std::map<std::string, CustomCommand*>::value_type ValType;
    
    /// Copying not allowed.
    ScriptInterpreter(const ScriptInterpreter&);
    /// Assignment not allowed.
    ScriptInterpreter& operator=(const ScriptInterpreter&);
    
    /// Map containing all custom commands for interpreter.
    std::map<std::string, CustomCommand*> commands_;
    /// The latest interpreter result. 
    DataObject* result_;
    /// Indicates whether we are in error state.
    bool errorState_;
    /// True if Interpreter is finalized.
    bool finalized_;
    /// LineReader for interpreter.
    LineReader* reader_;
};

#endif
