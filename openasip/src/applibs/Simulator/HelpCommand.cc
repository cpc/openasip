/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file HelpCommand.cc
 *
 * Declaration of HelpCommand class.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
HelpCommand::execute(const std::vector<DataObject>& arguments) {
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
