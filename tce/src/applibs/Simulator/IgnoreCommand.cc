/**
 * @file IgnoreCommand.cc
 *
 * Implementation of IgnoreCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "IgnoreCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimValue.hh"
#include "StopPointManager.hh"

#include <iostream>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
IgnoreCommand::IgnoreCommand() : 
    SimControlLanguageCommand("ignore") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
IgnoreCommand::~IgnoreCommand() {
}

/**
 * Executes the "ignore" command.
 *
 * Modifies the ignore counts of breakpoints.
 *
 * @param arguments The count of steps (default is one step).
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 * @todo Use the count for the step.
 */
bool 
IgnoreCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    if (!checkArgumentCount(arguments.size() - 1, 2, 2)) {
        return false;
    } 

    if  (!checkPositiveIntegerArgument(arguments[1]) ||
         !checkPositiveIntegerArgument(arguments[2])) {
        return false;
    }

    if (!checkSimulationStopped()) {
        if (!checkSimulationInitialized()) {
            return false;
        }
    }

    const int breakpointHandle = arguments[1].integerValue();
    const int ignoreCount = arguments[2].integerValue();
    try {
        StopPointManager& stopPointManager =
            simulatorFrontend().stopPointManager();

        // just check that breakpoint is found with the handle
        stopPointManager.stopPointWithHandleConst(breakpointHandle);
        stopPointManager.setIgnore(breakpointHandle, ignoreCount);
        
        printBreakpointInfo(breakpointHandle);
    } catch (const InstanceNotFound&) {
        interpreter()->setError(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_BREAKPOINT_NOT_FOUND).str());
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
 * @todo Use SimulatorTextGenerator to get the help text.
 */
std::string 
IgnoreCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_IGNORE).str();
}
