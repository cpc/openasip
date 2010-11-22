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
        vector<TCEString> commands = StringTools::chopString(commandLine, " ");
        
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
