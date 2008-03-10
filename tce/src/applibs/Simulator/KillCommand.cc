/**
 * @file KillCommand.cc
 *
 * Implementation of KillCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "KillCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
KillCommand::KillCommand() : 
    SimControlLanguageCommand("kill") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
KillCommand::~KillCommand() {
}

/**
 * Executes the "kill" command.
 *
 * Terminate the simulation. The program being simulated remains 
 * loaded and the simulation can be restarted from the beginning 
 * by means of command run. The Simulator will normally prompt the user 
 * for comfirmation before terminating the simulation.
 *
 * @param arguments No arguments needed.
 * @return Always true.
 * @exception NumberFormatException Is never thrown by this command.
 *
 */
bool 
KillCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    const int argumentCount = arguments.size() - 1;
    if (!checkArgumentCount(argumentCount, 0, 0)) {
        return false;
    } 

    if (!checkSimulationEnded()) {
        if (!checkSimulationStopped()) {
            return false;
        }
    }

    if (!interpreter()->lineReader()->confirmation(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_CONFIRM_KILL_SIMULATION).str())) {
        return false;
    }

    simulatorFrontend().killSimulation();
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
KillCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_KILL).str();
}

