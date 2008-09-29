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
 * @file ScriptInterpreter.hh
 *
 * The declaration of ScriptInterpreter class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
    virtual std::string result() throw (NumberFormatException);

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
        const std::string& interpreterVariableName) 
        throw (NumberFormatException);
    virtual int variableIntegerValue(
        const std::string& interpreterVariableName)
        throw (NumberFormatException);
    
    virtual bool processScriptFile(const std::string& scriptFileName)
        throw (UnreachableStream);

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
        const std::string& name, 
        const DataObject& value) 
        throw (NumberFormatException) = 0;
    virtual DataObject variable(const std::string& name) = 0;
    virtual bool interpret(const std::string& commandLine) = 0;
    virtual void setResultToInterpreter(const DataObject& value) 
        throw (NumberFormatException) = 0;

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
