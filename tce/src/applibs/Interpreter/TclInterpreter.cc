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
 * @file TclInterpreter.cc
 *
 * The definition of TclInterpreter class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 27 May 2004 by pj, jn, vpj, ll
 * @note rating: yellow
 */

#include <vector>
#include <string>

#include "TclInterpreter.hh"
#include "Conversion.hh"
#include "StringTools.hh"
#include "Application.hh"

using std::vector;
using std::string;

/**
 * Constructor.
 */
TclInterpreter::TclInterpreter() : 
    ScriptInterpreter(), context_(NULL), interpreter_(NULL) {
}

/**
 * Destructor.
 */
TclInterpreter::~TclInterpreter() {
    Tcl_DeleteInterp(interpreter_);
}

/**
 * Initializes the interpreter.
 *
 * Interpreter instance is created, as well as argc and argv variables are
 * set in interpreter.
 *
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 * @param context The context for interpreter.
 * @param reader LineReader for the interpreter.
 */
void
TclInterpreter::initialize(
    int argc, 
    char* argv[], 
    InterpreterContext* context,
    LineReader* reader) {

    setLineReader(reader);
    context_ = context;
    interpreter_ = Tcl_CreateInterp();

    if (argc == 0 || argv == NULL) {
	return;
    }
    
    // argv is set to interpreter
    char* args = Tcl_Merge(argc-1, (char **)argv+1);
    char* argvStr = const_cast<char*>("argv");
    Tcl_SetVar(interpreter_, argvStr, args, TCL_GLOBAL_ONLY);
    ckfree(args);
    
    // argc is set to interpreter
    string buffer = Conversion::toString(argc-1);
    setVariable("argc", buffer);
    setVariable("argv0", argv[0]);
}

/**
 * Sets a variable for interpreter.
 *
 * @param name The name of the variable.
 * @param value The value for a variable.
 * @exception NumberFormatException If converting value to string fails.
 */
void
TclInterpreter::setVariableToInterpreter(
    const std::string& name, const DataObject& value) {
    char* cName = StringTools::stringToCharPtr(name);
    char* cValue = StringTools::stringToCharPtr(value.stringValue());
    Tcl_SetVar(interpreter_, cName, cValue, TCL_GLOBAL_ONLY);
    delete[] cName;
    delete[] cValue;
}

/**
 * Returns the DataObject corresponding a certain variable.
 *
 * @param name The name of the variable.
 * @return The DataObject which holds the value of variable.
 */
DataObject
TclInterpreter::variable(const std::string& name) {
    DataObject object;
    char* cName = StringTools::stringToCharPtr(name);
    string value = Tcl_GetVar(interpreter_, cName, TCL_GLOBAL_ONLY);
    delete[] cName;
    object.setString(value);
    return object;
}

/**
 * Interprets one command line.
 *
 * @param commandLine The line to be interpreted.
 * @return True if interpreting is successful, false otherwise.
 */
bool
TclInterpreter::interpret(const std::string& commandLine) {

    LineReader* reader = lineReader();
    StringTools::trim(commandLine);
    string command = StringTools::trim(commandLine);
    if (!command.empty() && command.substr(command.length() - 1, 1) == "{" &&
        reader != NULL) {
        // command line ended with '{' so let's wait for more input
        /// @todo This does not work with nested loops correctly!
        /// It stops after it encouters the first line with only "}",
        /// thus it stops after the nested loop definition stops.
        string line = "";
        do {
            line = StringTools::trim(reader->readLine("  "));
            StringTools::trim(line);
            command += "\n" + line;
        } while (!line.empty() && line.substr(line.length() - 1, 1) != "}");
    }

    char* cCommandLine = StringTools::stringToCharPtr(command);
    int code = Tcl_Eval(interpreter_, cCommandLine);
    delete[] cCommandLine;
    
    Tcl_Obj* object = Tcl_GetObjResult(interpreter_);
    DataObject* dObject = new DataObject(tclObjToDataObject(object));
    setResult(dObject);
    
    if (code == TCL_OK) {
        setError(false);
        return true;
    } else {
        setError(true);
        return false;
    }
}

/**
 * Sets result to concrete interpreter.
 *
 * @param value The value to be set.
 * @exception NumberFormatException If conversion to Tcl_Obj fails.
 */
void
TclInterpreter::setResultToInterpreter(const DataObject& value) {
    Tcl_Obj* object = dataObjectToTclObj(value);
    Tcl_SetObjResult(interpreter_, object);
}

/**
 * Adds CustomCommand to interpreter.
 *
 * @param command The command to be added.
 */
void
TclInterpreter::addCustomCommandToInterpreter(const CustomCommand& command) {
    char* cName = StringTools::stringToCharPtr(command.name());
    Tcl_CreateObjCommand(
        interpreter_, cName, customCommandRedirector, (ClientData)(&command), 
        NULL);
    delete[] cName;
}

/**
 * Removes custom command from interpreter.
 *
 * @param command The command to be removed.
 */
void
TclInterpreter::removeCustomCommandFromInterpreter(
    const CustomCommand& command) {

    char* cName = StringTools::stringToCharPtr(command.name());
    Tcl_DeleteCommand(interpreter_, cName);
    delete[] cName;
}

/**
 * Converts Tcl_Obj to DataObject.
 *
 * @param object The object to be converted.
 * @return DataObject which was converted from Tcl_Obj.
 */
DataObject
TclInterpreter::tclObjToDataObject(Tcl_Obj* object) {
    DataObject dObj;
    dObj.setString(Tcl_GetStringFromObj(object, NULL));
    return dObj;
}

/**
 * Converts DataObject to Tcl_Obj.
 *
 * @param object The object to be converted.
 * @return Tcl_Obj converted from DataObject.
 * @exception NumberFormatException If conversion fails.
 */
Tcl_Obj*
TclInterpreter::dataObjectToTclObj(const DataObject& object) {
    char* cName = StringTools::stringToCharPtr(object.stringValue());
    Tcl_Obj* obj = Tcl_NewStringObj(cName, -1);
    delete[] cName;
    return obj;
}

/**
 * Processes script file.
 *
 * @param scriptFileName The name of the script file.
 * @return True, if process is successful, false otherwise.
 * @exception UnreachableStream Don't throw it.
 */
bool
TclInterpreter::processScriptFile(const std::string& scriptFileName) {
    string command = "source " + scriptFileName;
    if (!interpret(command)) {
        return false;
    }
    return true;
}

/**
 * This handler function is used to handle all CustomCommands.
 *
 * ClientData field contains CustomCommand to be executed. Tcl_Objs are 
 * converted to DataObjects which are then passed to CustomCommand.
 *
 * @param cd ClientData field.
 * @param interp Not used.
 * @param objc The number of Tcl_Objs.
 * @param objv The Tcl_Objs.
 */
int
TclInterpreter::customCommandRedirector(
    ClientData cd,
    Tcl_Interp*,
    int objc,
    Tcl_Obj *CONST objv[]) {

    vector<DataObject> args;
    
    for (int i = 0; i < objc; i++) {
        DataObject dObj = TclInterpreter::tclObjToDataObject(objv[i]);
        args.push_back(dObj);
    }

    CustomCommand* targetCommand = static_cast<CustomCommand*>(cd);
    assert(cd != NULL);

    if (targetCommand->execute(args)) {
        return TCL_OK;
    }
    return TCL_ERROR;
}

/**
 * Returns the context of the interpreter.
 *
 * @return The context of the interpreter.
 */
InterpreterContext&
TclInterpreter::context() const {
    return *context_;
}
