/**
 * @file DisableBPCommand.cc
 *
 * Implementation of DisableBPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisableBPCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimValue.hh"
#include "StopPointManager.hh"
#include "StringTools.hh"

#include <iostream>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
DisableBPCommand::DisableBPCommand() : 
    SimControlLanguageCommand("disablebp") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
DisableBPCommand::~DisableBPCommand() {
}

/**
 * Executes the "disablebp" command.
 *
 * Disables the given breakpoints.
 *
 * @param arguments The handle(s) of the breakpoint(s) to enable.
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 * @todo Use the count for the step.
 */
bool 
DisableBPCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    if (!checkArgumentCount(arguments.size() - 1, 1, INT_MAX)) {
        return false;
    } 

    if (!checkSimulationStopped()) {
        if (!checkSimulationInitialized()) {
            return false;
        }
    }

    StopPointManager& breakpointManager =
        simulatorFrontend().stopPointManager();
    
    if (!verifyBreakpointHandles(arguments)) {
        return false;
    }

    // disable the given breakpoints
    for (size_t i = 1; i < arguments.size(); ++i) {
        const unsigned int breakpointHandle = arguments[i].integerValue();    
        breakpointManager.disable(breakpointHandle);
        printBreakpointInfo(breakpointHandle);
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
DisableBPCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_DISABLEBP).str();
}
