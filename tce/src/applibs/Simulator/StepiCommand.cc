/**
 * @file StepiCommand.cc
 *
 * Implementation of StepiCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "StepiCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimValue.hh"

#include <iostream>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
StepiCommand::StepiCommand() : SimControlLanguageCommand("stepi") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
StepiCommand::~StepiCommand() {
}

/**
 * Executes the "stepi" command.
 *
 * Executes the user defined count of next TTA instruction and returns 
 * control back to user.
 *
 * @param arguments The count of steps (default is one step).
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 * @todo Use the count for the step.
 */
bool 
StepiCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    if (!checkArgumentCount(arguments.size() - 1, 0, 1)) {
        return false;
    } 

    double stepCount = 1;
    if (arguments.size() == 2) {
        if  (!checkDoubleArgument(arguments[1])) {
            return false;
        }
        stepCount = arguments[1].doubleValue();
    }

    if (!checkSimulationStopped()) {
        if (!checkSimulationInitialized()) {
            return false;
        }
    }

    simulatorFrontend().step(stepCount);
    printNextInstruction();
    printSimulationTime();
    return true;
}

/**
 * Returns the help text for this command.
 * 
 * Help text is searched from SimulatorTextGenerator.
 *
 * @return The help text.
 * @todo Use SimulatorTextGenerator to get the help text.
 */
std::string 
StepiCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_STEPI).str();
}
