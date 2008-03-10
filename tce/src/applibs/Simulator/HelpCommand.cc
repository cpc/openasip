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
