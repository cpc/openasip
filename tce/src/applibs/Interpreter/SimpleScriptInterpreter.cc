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
 * @file SimpleScriptInterpreter.cc
 *
 * Definition of SimpleScriptInterpreter class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <vector>

#include "SimpleScriptInterpreter.hh"
#include "StringTools.hh"
#include "InterpreterContext.hh"
#include "LineReader.hh"
#include "Application.hh"

using std::string;
using std::vector;

/**
 * Constructor.
 */
SimpleScriptInterpreter::SimpleScriptInterpreter() : ScriptInterpreter() {
}

/**
 * Destructor.
 */
SimpleScriptInterpreter::~SimpleScriptInterpreter() {
}

/**
 * Initializes the interpreter.
 *
 * @param context Context for interpreter.
 * @param reader LineReader for interpreter. 
 */
void
SimpleScriptInterpreter::initialize(
    int, 
    char*[], 
    InterpreterContext* context, 
    LineReader* reader) {

    context_ = context;
    setLineReader(reader);
}

/**
 * Sets variable to interpreter.
 *
 * @param name The name of the variable.
 * @param value The value of the variable.
 * @exception NumberFormatException Cannot throw.
 */
void
SimpleScriptInterpreter::setVariableToInterpreter(
    const std::string& name,
    const DataObject& value) 
    throw (NumberFormatException) {

    VariableMap::iterator iter = variables_.find(name);
    
    if (iter == variables_.end()) {
        variables_[name] = value.stringValue();
    } else {
        (*iter).second = value.stringValue();
    }
}

/**
 * Returns the variable with the given name.
 *
 * If variable is not found, returns uninitialized data object.
 *
 * @param name The name of the variable.
 * @return The data object that holds the value of the varible.
 */
DataObject
SimpleScriptInterpreter::variable(const std::string& name) {
    DataObject object;
    VariableMap::iterator iter = variables_.find(name);
    if (iter != variables_.end()) {
        object.setString((*iter).second);
    }
    return object;
}

/**
 * Interprets a given command line.
 *
 * @param commandLine The command line to interpreted.
 * @return True if interpreting was successful, false otherwise.
 */
bool
SimpleScriptInterpreter::interpret(const std::string& commandLine) {

    string line = StringTools::trim(commandLine);
    if (commandLine != "") {
        vector<string> commands = StringTools::chopString(commandLine, " ");
        
        CustomCommand* custCommand = customCommand(commands[0]);
        if (custCommand == NULL) {
            DataObject* result = new DataObject();
            string msg = "Unknown command: " + commands[0];
            result->setString(msg);
            setResult(result);
            return false;
        }
        
        vector<DataObject> args;
        for (unsigned int i = 0; i < commands.size(); i++) {
            DataObject obj;
            obj.setString(commands[i]);
            args.push_back(obj);
        }

        bool res;
        try {
            res = custCommand->execute(args);
        } catch (const NumberFormatException& n) {
            DataObject* result = new DataObject();
            result->setString(n.errorMessage());
            setResult(result);
            res = false;        
        }

        if (res) {
            setError(false);
            return true;
        } else {
            setError(true);
            return false;
        }
    } else {
        // command line was empty
        DataObject* result = new DataObject();
        result->setString("");
        setResult(result);
        setError(false);
        return true;
    }
    
    assert(false);
    return false;
}

/**
 * This function does nothing, because result is already stored
 * in ScriptInterpreter.
 *
 * @exception Cannot throw.
 */
void
SimpleScriptInterpreter::setResultToInterpreter(const DataObject&) 
    throw (NumberFormatException) {
}

/**
 * Returns an instance of InterpreteContext.
 *
 * @return InterpreterContext.
 */
InterpreterContext&
SimpleScriptInterpreter::context() const {
    return *context_;
}

/**
 * Does nothing.
 */
void
SimpleScriptInterpreter::addCustomCommandToInterpreter(const CustomCommand&) {
}

/**
 * Does nothing.
 */
void
SimpleScriptInterpreter::removeCustomCommandFromInterpreter(
    const CustomCommand&) {
}
