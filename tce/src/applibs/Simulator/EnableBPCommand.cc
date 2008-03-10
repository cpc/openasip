/**
 * @file EnableBPCommand.cc
 *
 * Implementation of EnableBPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "EnableBPCommand.hh"
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
EnableBPCommand::EnableBPCommand() : 
    SimControlLanguageCommand("enablebp") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
EnableBPCommand::~EnableBPCommand() {
}

/**
 * Executes the "enablebp" command.
 *
 * Enables the given breakpoints.
 *
 * @param arguments The handle(s) of the breakpoint(s) to enable.
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 * @todo Use the count for the step.
 */
bool 
EnableBPCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    if (!checkArgumentCount(arguments.size() - 1, 1, INT_MAX)) {
        return false;
    } 

    if (!checkSimulationStopped()) {
        if (!checkSimulationInitialized()) {
            return false;
        }
    }

    const std::string firstArgument = 
        StringTools::trim(StringTools::stringToLower(
                              arguments[1].stringValue()));
    
    bool enableOnce = false;
    bool temporary = false;
    int firstHandleIndex = 1;
    if (firstArgument == "once") {
        enableOnce = true;
        firstHandleIndex = 2;
    } else if (firstArgument == "delete") {
        temporary = true;
        firstHandleIndex = 2;
    }

    StopPointManager& breakpointManager =
        simulatorFrontend().stopPointManager();
    
    if (!verifyBreakpointHandles(arguments, firstHandleIndex)) {
        return false;
    }

    // enable (and modify the properties of) the given breakpoints
    for (size_t i = firstHandleIndex; i < arguments.size(); ++i) {
        const unsigned int breakpointHandle = arguments[i].integerValue();    
        breakpointManager.enable(breakpointHandle);
        if (enableOnce) {
            breakpointManager.enableOnceAndDisable(breakpointHandle);
        }
        if (temporary) {
            breakpointManager.enableOnceAndDelete(breakpointHandle);
        }
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
EnableBPCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_ENABLEBP).str();
}
