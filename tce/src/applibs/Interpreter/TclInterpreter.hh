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
        const std::string& name, 
        const DataObject& value)
        throw (NumberFormatException);
    virtual DataObject variable(const std::string& name);
    virtual bool interpret(const std::string& commandLine);
    virtual void setResultToInterpreter(const DataObject& value)
        throw (NumberFormatException);
   
    static DataObject tclObjToDataObject(Tcl_Obj* object);
    static Tcl_Obj* dataObjectToTclObj(const DataObject& object)
        throw (NumberFormatException);

    virtual bool processScriptFile(const std::string& scriptFileName)
        throw (UnreachableStream);

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
