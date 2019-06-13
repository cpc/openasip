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
 * @file TclInterpreter.hh
 *
 * The declaration of TclInterpreter class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note reviewed 27 May 2004 by pj, jn, vpj, ll
 * @note rating: yellow
 */

#ifndef TTA_TCL_INTERPRETER_HH
#define TTA_TCL_INTERPRETER_HH

#include <string>
#include <tcl.h>

#include "ScriptInterpreter.hh"
#include "Exception.hh"

/**
 * Tcl implementation of ScriptInterpreter.
 *
 * Tcl (Tool command language) is a very simple programming language. In TCE
 * it is used for command language for Simulator. (Maybe for some other 
 * application too.)
 *
 * Is able to interpret all tcl commands and user defined CustomCommands.
 */
class TclInterpreter : public ScriptInterpreter {
public:
    TclInterpreter();
    virtual ~TclInterpreter();

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

    static DataObject tclObjToDataObject(Tcl_Obj* object);
    static Tcl_Obj* dataObjectToTclObj(const DataObject& object);

    virtual bool processScriptFile(const std::string& scriptFileName);

    static int customCommandRedirector(
        ClientData cd,
        Tcl_Interp* interp,
        int objc, 
        Tcl_Obj *CONST objv[]);

    virtual InterpreterContext& context() const;

protected:
    virtual void addCustomCommandToInterpreter(const CustomCommand& command);
    virtual void removeCustomCommandFromInterpreter(
        const CustomCommand& command);

private:
    /// Copying not allowed.
    TclInterpreter(const TclInterpreter&);
    /// Assignment not allowed
    TclInterpreter& operator=(const TclInterpreter&);

    /// Context for interpreter.
    InterpreterContext* context_;
    /// Interpreter instance.
    Tcl_Interp* interpreter_;

};

#endif
