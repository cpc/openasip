/**
 * @file SimControlLanguageCommand.hh
 *
 * Declaration of SimControlLanguageCommand class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMCONLANG_COMMAND
#define TTA_SIMCONLANG_COMMAND

#include <string>
#include <vector>
#include <iostream>

#include "DataObject.hh"
#include "CustomCommand.hh"
#include "Exception.hh"
#include "SimulatorConstants.hh"
#include "Address.hh"

class SimulatorFrontend;
class Breakpoint;
class TclConditionScript;
class ExpressionScript;

///////////////////////////////////////////////////////////////////////////////
// SimControlLanguageCommand
///////////////////////////////////////////////////////////////////////////////

/**
 * This is a base class for Simulator Control Language commands.
 *
 * Provides services command and useful for all Simulator Control Language
 * commands.
 */
class SimControlLanguageCommand : public CustomCommand {
public:
    SimControlLanguageCommand(const std::string& name);
    virtual ~SimControlLanguageCommand();

    SimulatorFrontend& simulatorFrontend();
    const SimulatorFrontend& simulatorFrontendConst();
    virtual void printNextInstruction();
    virtual void printStopInformation();
    virtual void printStopReasons();
    virtual bool printBreakpointInfo(unsigned int breakpointHandle);
    virtual void printSimulationTime();

    virtual std::ostream& outputStream();

    bool checkSimulationInitialized();
    bool checkSimulationNotAlreadyRunning();
    bool checkSimulationStopped();
    bool checkSimulationEnded();
    bool checkParallelSimulation();
    bool checkProgramLoaded();

    InstructionAddress parseInstructionAddressExpression(
        const std::string& expression)
        throw (IllegalParameters);    

    TTAProgram::Address parseDataAddressExpression(
        const std::string& expression)
        throw (IllegalParameters);    

    bool parseBreakpoint(
        const std::vector<DataObject>& arguments, 
        Breakpoint& target);

    bool askConditionFromUser(TclConditionScript& target);
    bool askExpressionFromUser(ExpressionScript& target);

    bool verifyBreakpointHandles(
        const std::vector<DataObject>& arguments, 
        std::size_t startIndex = 1);
};

///////////////////////////////////////////////////////////////////////////////
// SimControlLanguageSubCommand
///////////////////////////////////////////////////////////////////////////////

/**
 * Base class for classes that implement subcommands of Simulator Control 
 * Language.
 */
class SimControlLanguageSubCommand {
public:
    SimControlLanguageSubCommand(SimControlLanguageCommand& parentCommand);
    virtual ~SimControlLanguageSubCommand();
    virtual bool execute(const std::vector<DataObject>& arguments) = 0;
    virtual SimControlLanguageCommand& parent();
private:
    /// the main command
    SimControlLanguageCommand& parentCommand_;    
};


#endif
