/**
 * @file RunCommand.cc
 *
 * Implementation of RunCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "RunCommand.hh"
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
RunCommand::RunCommand() : SimControlLanguageCommand("run") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
RunCommand::~RunCommand() {
}

/**
 * Executes the "run" command.
 *
 * Starts the simulation.
 *
 * @param arguments No arguments needed.
 * @return Always true.
 * @exception NumberFormatException Is never thrown by this command.
 *
 */
bool 
RunCommand::execute(const std::vector<DataObject>&)
    throw (NumberFormatException) {
        
    if (!checkSimulationNotAlreadyRunning() ||
        !checkSimulationInitialized()) {
        
        return false;
    }
    
    simulatorFrontend().run();
    
    printStopInformation();

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
RunCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_RUN).str();
}

