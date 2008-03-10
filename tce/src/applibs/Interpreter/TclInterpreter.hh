/**
 * @file TclInterpreter.hh
 *
 * The declaration of TclInterpreter class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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
