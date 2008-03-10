/**
 * @file UntilCommand.cc
 *
 * Implementation of UntilCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "UntilCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimControlLanguageCommand.hh"
#include "SimValue.hh"
#include "Procedure.hh"

#include <iostream>
#include <boost/timer.hpp>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
UntilCommand::UntilCommand() : SimControlLanguageCommand("until") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
UntilCommand::~UntilCommand() {
}

/**
 * Executes the "until" command.
 *
 * Run until the program location specified by arg is reached and stop
 * before executing the instruction at specified address. Any valid 
 * argument that applies to command break is also a valid argument for until. 
 *
 * @param arguments The count of steps (default is one step).
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 */
bool 
UntilCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {
    
    boost::timer time;
    
    const int argumentCount = arguments.size() - 1;
    if (!checkArgumentCount(argumentCount, 0, 1)) {
        return false;
    } 

    if (!checkSimulationInitialized() && !checkSimulationStopped()) {
        return false;
    }

    try {
        const std::string argument = 
            ((argumentCount == 1)?(arguments[1].stringValue()):(""));

        const int instructionAddress = 
            parseInstructionAddressExpression(argument);
        simulatorFrontend().runUntil(instructionAddress);
        printNextInstruction();
        printSimulationTime();
    } catch (const IllegalParameters&) {
        return false;
    }
    return true;
}

/**
 * Returns the help text for this command.
 * 
 * Help text is searched from SimulatorTextGenerator.
 *
 * @return The help text.
 */
std::string 
UntilCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_UNTIL).str();
}
