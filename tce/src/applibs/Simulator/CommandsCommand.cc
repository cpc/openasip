/**
 * @file CommandsCommand.cc
 *
 * Implementation of CommandsCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "CommandsCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimValue.hh"
#include "StringTools.hh"

#include <iostream>
#include <algorithm>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
CommandsCommand::CommandsCommand() : 
    SimControlLanguageCommand("commands") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
CommandsCommand::~CommandsCommand() {
}

/**
 * Executes the "commands" command.
 *
 * Displays the given count of entries in the command history log.
 *
 * @param arguments The count of entries to be printed.
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 * @todo Use the count for the step.
 */
bool 
CommandsCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    if (!checkArgumentCount(arguments.size() - 1, 0, 1)) {
        return false;
    } 

    std::size_t commandCount = 10;
    if (arguments.size() == 2) {
        if  (!checkPositiveIntegerArgument(arguments[1])) {
            return false;
        }
        commandCount = arguments[1].integerValue();
    }

    commandCount = std::min(
        commandCount, interpreter()->lineReader()->inputsInHistory());
    
    for (int i = commandCount - 1; i >= 0; --i) {
        outputStream() 
            << StringTools::trim(
                interpreter()->lineReader()->inputHistoryEntry(i))
            << std::endl;
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
CommandsCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_COMMANDS).str();
}
