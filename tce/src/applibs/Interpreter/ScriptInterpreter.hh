/**
 * @file ScriptInterpreter.hh
 *
 * The declaration of ScriptInterpreter class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
