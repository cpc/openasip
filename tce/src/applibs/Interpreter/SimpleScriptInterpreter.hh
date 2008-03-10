/**
 * @file SimpleScriptInterpreter.hh
 *
 * Declaration of SimpleScriptInterpreter.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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
        const std::string& name,
        const DataObject& value
        ) throw (NumberFormatException);

    virtual DataObject variable(const std::string& name);
    virtual bool interpret(const std::string& commandLine);
    virtual void setResultToInterpreter(const DataObject& value)
        throw (NumberFormatException);

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
