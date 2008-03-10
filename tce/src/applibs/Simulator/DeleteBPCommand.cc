/**
 * @file DeleteBPCommand.cc
 *
 * Implementation of DeleteBPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DeleteBPCommand.hh"
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
DeleteBPCommand::DeleteBPCommand() : 
    SimControlLanguageCommand("deletebp") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
DeleteBPCommand::~DeleteBPCommand() {
}

/**
 * Executes the "deletebp" command.
 *
 * Deletes the given breakpoints.
 *
 * @param arguments The handle(s) of the breakpoint(s) to delete.
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 * @todo Use the count for the step.
 */
bool 
DeleteBPCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    if (!checkSimulationStopped()) {
        if (!checkSimulationInitialized()) {
            return false;
        }
    }

    StopPointManager& breakpointManager =
        simulatorFrontend().stopPointManager();
    if (arguments.size() > 1) {
        if (!verifyBreakpointHandles(arguments)) {
            return false;
        }

        // delete the given breakpoints
        for (size_t i = 1; i < arguments.size(); ++i) {
            const unsigned int breakpointHandle = arguments[i].integerValue();    
            try {
                breakpointManager.deleteStopPoint(breakpointHandle);
            } catch (const InstanceNotFound&) {
                // user defined the same breakpoint handle twice, let's
                // ignore this
            }
        }
    } else {
        // delete all breakpoints
        if (!interpreter()->lineReader()->confirmation(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_CONFIRM_DELETE_ALL_BREAKPOINTS).str())) {
            // user answered "no", command is still considered succesful
            return true;
        }
        breakpointManager.deleteAll();
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
DeleteBPCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_DELETEBP).str();
}
