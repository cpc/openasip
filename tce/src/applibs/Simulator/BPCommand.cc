/**
 * @file BPCommand.cc
 *
 * Implementation of BPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "BPCommand.hh"
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
BPCommand::BPCommand() : SimControlLanguageCommand("bp") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
BPCommand::~BPCommand() {
}

/**
 * Executes the "bp" command.
 *
 * A breakpoint stops the simulation whenever the Simulator reaches 
 * a certain point in the program. It is possible to add a condition to 
 * a breakpoint, to control when the Simulator must stop with increased 
 * precision. 
 *
 * @param arguments The address (and condition of the breakpoint).
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 */
bool 
BPCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    Breakpoint breakpoint(simulatorFrontend(), 0);
    if (!parseBreakpoint(arguments, breakpoint)) {
        
        return false;
    }

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
BPCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_BREAK).str();
}
