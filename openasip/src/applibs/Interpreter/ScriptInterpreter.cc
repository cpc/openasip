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
 * @file ScriptInterpreter.cc
 *
 * Definition of ScriptInterpreter class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note reviewed 27 May 2004 by pj, jn, vpj, ll
 * @note rating: yellow
 */

#include <fstream>
#include <algorithm>

#include "ScriptInterpreter.hh"
#include "MapTools.hh"
#include "Exception.hh"
#include "Application.hh"

using std::string;
using std::ifstream;
using std::vector;
using std::stable_sort;

/**
 * Constructor.
 */
ScriptInterpreter::ScriptInterpreter() : 
    result_(NULL), errorState_(false), finalized_(false) { 
}

/**
 * Destructor.
 */
ScriptInterpreter::~ScriptInterpreter() {

    if (!finalized_) {
        string method = "ScriptInterpreter::~ScriptInterpreter()";
        string message = "Interpreter not finalized.";
        Application::writeToErrorLog(__FILE__, __LINE__, method, message);
    }

    MapTools::deleteAllValues(commands_);

    if (result_ != NULL) {
        delete result_;
    }
}

/**
 * Adds a custom command to interpreter.
 *
 * @param command The command to be added.
 */
void
ScriptInterpreter::addCustomCommand(CustomCommand* command) {
    command->setInterpreter(this);
    commands_.insert(ValType(command->name(), command));
    addCustomCommandToInterpreter(*command);
}

/**
 * Removes a custom command from interpreter.
 *
 * Reserved memory for CustomCommand is freed.
 * 
 * @param commandName The name of the command that is removed.
 */
void
ScriptInterpreter::removeCustomCommand(const std::string& commandName) {
    if (MapTools::containsKey(commands_, commandName)) {
        MapIter mi = commands_.find(commandName);
        removeCustomCommandFromInterpreter(*(*mi).second);
        delete (*mi).second;
        commands_.erase(mi);
    }
}

/**
 * Returns a custom command with a given name.
 *
 * Returns NULL if CustomCommand is not found.
 * 
 * @param commandName The name of the wanted command.
 * @return The pointer to a wanted custom command.
 */
CustomCommand*
ScriptInterpreter::customCommand(const std::string& commandName) {
    
    if (!MapTools::containsKey(commands_, commandName)) {
        return NULL;
    }

    MapIter mi = commands_.find(commandName);
    return (*mi).second;
}

/**
 * Stores the result of last executed command.
 *
 * Sets result also to concrete interpreter.
 *
 * @param result The result.
 */
void
ScriptInterpreter::setResult(DataObject* result) {
    if (result_ != NULL) {
        delete result_;
    }
    result_ = result;
    setResultToInterpreter(*result);
}

/**
 * Stores the result of last executed command.
 *
 * Sets result also to concrete interpreter.
 *
 * @param result The result.
 */
void
ScriptInterpreter::setResult(const std::string& result) {
    if (result_ == NULL) {
        result_ = new DataObject();
    }
    result_->setString(result);
    setResultToInterpreter(*result_);
}

/**
 * Stores the result of last executed command.
 *
 * Sets result also to concrete interpreter.
 *
 * @param result The result.
 */
void
ScriptInterpreter::setResult(int result) {
    if (result_ == NULL) {
        result_ = new DataObject();
    }
    result_->setInteger(result);
    setResultToInterpreter(*result_);
}

/**
 * Stores the result of last executed command.
 *
 * Sets result also to concrete interpreter.
 *
 * @param result The result.
 */
void
ScriptInterpreter::setResult(double result) {
    if (result_ == NULL) {
        result_ = new DataObject();
    }
    result_->setDouble(result);
    setResultToInterpreter(*result_);
}

/**
 * Returns the result of last executed command as a string.
 *
 * @return Last result as string.
 * @exception NumberFormatException If converting result to string fails.
 */
string
ScriptInterpreter::result() {
    if (result_ != NULL) {
        return result_->stringValue();
    } else {
        return "";
    }
}

/**
 * Sets or unsets the error state of interpreter.
 *
 * @param state The error state flag.
 */
void
ScriptInterpreter::setError(bool state) {
    errorState_ = state;
}

/**
 * Sets an error message and sets errors status of interpreter.
 *
 * @param errorMessage The error message.
 */
void
ScriptInterpreter::setError(std::string errorMessage) {
    DataObject* result = new DataObject();
    result->setString(errorMessage);
    setResult(result);
    setError(true);
}


/**
 * Returns true, if interpreter is in error state.
 *
 * @return True, if interpreter is in error state.
 */
bool
ScriptInterpreter::error() const {
    return errorState_;
}

/**
 * Sets value for a variable.
 *
 * @param interpreterVariableName The name of the variable.
 * @param value The value for a variable.
 */
void
ScriptInterpreter::setVariable(
    const std::string& interpreterVariableName, 
    const std::string& value) {
    
    DataObject object;
    object.setString(value);
    setVariableToInterpreter(interpreterVariableName, object);
}

/**
 * Sets the value for interpreter variable.
 *
 * @param interpreterVariableName The name of the variable.
 * @param value The value for the variable.
 */
void
ScriptInterpreter::setVariable(
    const std::string& interpreterVariableName, 
    int value) {
    
    DataObject object;
    object.setInteger(value);
    setVariableToInterpreter(interpreterVariableName, object);
}

/**
 * Returns the value of the string variable.
 *
 * @param interpreterVariableName The name of the variable.
 * @return The value of the variable.
 * @exception NumberFormatException If converting variable to string fails.
 */
string
ScriptInterpreter::variableStringValue(
    const std::string& interpreterVariableName) {
    DataObject var = variable(interpreterVariableName);
    string value = var.stringValue();
    return value;
}

/**
 * Returns the value of the integer variable.
 *
 * @param interpreterVariableName The name of the variable.
 * @return The value of the variable.
 * @exception NumberFormatException If converting variable to integer fails.
 */
int
ScriptInterpreter::variableIntegerValue(
    const std::string& interpreterVariableName) {
    DataObject var = variable(interpreterVariableName);
    int value = var.integerValue();
    return value;
}

/**
 * Opens a script file and processes it.
 *
 * @param scriptFileName The name of the script file.
 * @return True, if process is successful, false otherwise.
 * @exception UnreachableStream If file cannot be opened.
 */
bool
ScriptInterpreter::processScriptFile(const std::string& scriptFileName) {
    ifstream fileStream(scriptFileName.c_str());
    
    if (fileStream.bad()) {
        string method = "ScriptInterpreter::processScriptFile";
        string message = "Cannot open file " + scriptFileName;
        throw UnreachableStream(__FILE__, __LINE__, method, message);
    }

    string line;
    while (getline(fileStream, line)) {
        if (!interpret(line)) {
            fileStream.close();
            return false;
        }
    }
    fileStream.close();
    return true;
}

/**
 * Removes all created CustomCommands from interpreter.
 *
 * This is called before interpreter is deleted.
 */
void
ScriptInterpreter::finalize() {
    for (MapIter mi = commands_.begin(); mi != commands_.end(); mi++) {
        removeCustomCommandFromInterpreter(*(*mi).second);
    }
    finalized_ = true;
}

/**
 * Returns a vector of alphabetically sorted names of CustomCommands.
 *
 * @return CustomCommands sorted by name.
 */
vector<string>
ScriptInterpreter::customCommandsSortedByName() {
	
	vector<string> names;
	
	MapIter it = commands_.begin();
	while (it != commands_.end()) {
		names.push_back((*it).second->name());
		it++;
	}

	vector<string>::iterator first = names.begin();
	vector<string>::iterator last = names.end();
	stable_sort(first , last);
	return names;
}

/**
 * Returns the LineReader instance.
 *
 * LineReader is needed by some CustomCommands. ScriptInterpreter may also 
 * need it.
 *  
 * @return LineReader instance.
 */
LineReader*
ScriptInterpreter::lineReader() const {
    return reader_;
}

/**
 * Sets the LineReader for the interpreter.
 *
 * @param reader The LineReader.
 */
void
ScriptInterpreter::setLineReader(LineReader* reader) {
    reader_ = reader;
}
