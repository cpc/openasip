/**
 * @file CustomCommand.hh
 *
 * The declaration of CustomCommand class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note reviewed 27 May 2004 by pj, jn, vpj, ll
 * @note rating: green
 */

#ifndef TTA_CUSTOM_COMMAND_HH 
#define TTA_CUSTOM_COMMAND_HH

#include <string>
#include <vector>

#include "LineReader.hh"
#include "InterpreterContext.hh"
#include "ScriptInterpreter.hh"
#include "DataObject.hh"
#include "Exception.hh"

class ScriptInterpreter;

/**
 * Abstract base class for all CustomCommands for Interpreter.
 *
 * CustomCommand is a user defined command designed for a particular task.
 */
class CustomCommand {

public:
    explicit CustomCommand(std::string name);
    CustomCommand(const CustomCommand& cmd);
    virtual ~CustomCommand();

    std::string name() const;

    void setContext(InterpreterContext* context);
    InterpreterContext* context() const;

    void setInterpreter(ScriptInterpreter* si);
    ScriptInterpreter* interpreter() const;

    virtual bool execute(const std::vector<DataObject>& arguments)
        throw (NumberFormatException) = 0;
    virtual std::string helpText() const = 0;

    bool checkArgumentCount(int argumentCount, int minimum, int maximum);
    bool checkIntegerArgument(const DataObject& argument);    
    bool checkPositiveIntegerArgument(const DataObject& argument);
    bool checkUnsignedIntegerArgument(const DataObject& argument);
    bool checkDoubleArgument(const DataObject& argument);

private:
    /// Assignment not allowed.
    CustomCommand& operator=(const CustomCommand&);
    
    /// The name of the command.
    std::string name_;
    /// Context of the command.
    InterpreterContext* context_;
    /// Interpreter for the command.
    ScriptInterpreter* interpreter_;
};

#include "CustomCommand.icc"

#endif
