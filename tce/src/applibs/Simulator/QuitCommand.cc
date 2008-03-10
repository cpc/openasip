/**
 * @file QuitCommand.cc
 *
 * Implementation of QuitCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "QuitCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimulatorInterpreter.hh"

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
QuitCommand::QuitCommand() : CustomCommand("quit") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
QuitCommand::~QuitCommand() {
}

/**
 * Executes the "quit" command.
 *
 * This method does the uninitialization of the simulator.
 *
 * @param arguments No arguments needed.
 * @return Always true.
 * @exception NumberFormatException Is never thrown by this command.
 *
 */
bool 
QuitCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {
    const int argumentCount = arguments.size() - 1;
    if (!checkArgumentCount(argumentCount, 0, 0)) {
        return false;
    } 
    dynamic_cast<SimulatorInterpreter*>(interpreter())->setQuitCommandGiven();
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
QuitCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_QUIT).str();
}

