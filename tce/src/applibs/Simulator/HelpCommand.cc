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
 * @file HelpCommand.cc
 *
 * Declaration of HelpCommand class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <vector>
#include <string>

#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "HelpCommand.hh"
#include "Application.hh"

using std::vector;
using std::string;

/**
 * Constructor.
 */
HelpCommand::HelpCommand() : SimControlLanguageCommand("help") {
}

/**
 * Destructor.
 */
HelpCommand::~HelpCommand() {
}

/**
 * Executes the help command.
 *
 * Prints a help message briefly describing command command. If no command
 * is given, prints general help and a listing of available commands.
 *
 * @param arguments Arguments for the command.
 * @return True if execution is succesfull, false otherwise
 * @exception NumberFormatException If conversion of DataObject fails.
 */
bool
HelpCommand::execute(const std::vector<DataObject>& arguments) 
    throw (NumberFormatException) {


    if (!checkArgumentCount(arguments.size() - 1, 0, 1)) {
        return false;
    } 

    if (arguments.size() == 1) {
        outputStream() << SimulatorToolbox::textGenerator().text(
                           Texts::TXT_CLI_ONLINE_HELP).str() 
                       << std::endl << std::endl;

        outputStream() << SimulatorToolbox::textGenerator().text(
            Texts::TXT_INTERP_HELP_COMMANDS_AVAILABLE).str() << std::endl;

		vector<string> names = interpreter()->customCommandsSortedByName();
		for (size_t i = 0; i < names.size(); i++) {
			outputStream() << names[i] + " ";
		}
        outputStream() << std::endl;
		return true;
	} 

    DataObject* obj = new DataObject();
    string cmdName = arguments[1].stringValue();
    CustomCommand* command = interpreter()->customCommand(cmdName);
    if (command == NULL) {
        obj->setString(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_UNKNOWN_COMMAND).str());
        interpreter()->setResult(obj);
        return false;
    } else {
        outputStream() << command->helpText() << std::endl;
        return true;
    }

    return true;
}

/**
 * Returns help text.
 *
 * @return Help text.
 * @todo TextGenerator.
 */
string
HelpCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_HELP).str();
}
