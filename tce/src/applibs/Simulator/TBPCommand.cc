/**
 * @file TBPCommand.cc
 *
 * Implementation of TBPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "TBPCommand.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimControlLanguageCommand.hh"
#include "StopPointManager.hh"
#include "Breakpoint.hh"

#include <iostream>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
TBPCommand::TBPCommand() : SimControlLanguageCommand("tbp") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
TBPCommand::~TBPCommand() {
}

/**
 * Executes the "tbp" command.
 *
 * Sets a temporary breakpoint, which is automatically deleted after 
 * the first time it stops the simulation. The arguments args are the 
 * same as for the bp command. 
 *
 * @param arguments The address (and condition of the breakpoint).
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 */
bool 
TBPCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    Breakpoint breakpoint(simulatorFrontend(), 0);
    if (!parseBreakpoint(arguments, breakpoint)) {
        return false;
    }
    breakpoint.setDeletedAfterTriggered(true);

    StopPointManager& stopPointManager = 
        simulatorFrontend().stopPointManager();
    return printBreakpointInfo(stopPointManager.add(breakpoint));
}

/**
 * Returns the help text for this command.
 * 
 * Help text is searched from SimulatorTextGenerator.
 *
 * @return The help text.
 */
std::string 
TBPCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_TBREAK).str();
}
