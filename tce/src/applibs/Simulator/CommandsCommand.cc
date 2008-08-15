/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
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
