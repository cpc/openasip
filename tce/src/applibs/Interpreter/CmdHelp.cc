/**
 * @file CmdHelp.cc
 *
 * Declaration of CmdHelp class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note reviewed 2 June 2004 by jm, pj, tr, jn
 * @note rating: green
 */

#include <vector>
#include <string>

#include "CmdHelp.hh"
#include "Application.hh"

using std::vector;
using std::string;


/**
 * Constructor.
 */
CmdHelp::CmdHelp() : CustomCommand("help") {
}

/**
 * Destructor.
 */
CmdHelp::~CmdHelp() {
}

/**
 * Executes the help command.
 *
 * @param arguments Arguments for the command.
 * @return True if execution is succesfull, false otherwise
 * @exception NumberFormatException If conversion of DataObject fails.
 */
bool
CmdHelp::execute(const std::vector<DataObject>& arguments) 
    throw (NumberFormatException) {
    
    ScriptInterpreter* interp = interpreter();
    assert(interp != NULL);
    DataObject* obj = new DataObject();
    
	if (arguments.size() == 1) {
		string result = "Commands available:\n";
		vector<string> names = interp->customCommandsSortedByName();
		for (size_t i = 0; i < names.size(); i++) {
			result += names[i] + " ";
		}
		obj->setString(result);
		interp->setResult(obj);
		return true;
	} 

    if (arguments.size() != 2) {
        obj->setString("Wrong number of arguments");
        interp->setResult(obj);
        return false;
    }
    
    string cmdName = arguments[1].stringValue();
    CustomCommand* command = interp->customCommand(cmdName);
    if (command == NULL) {
        obj->setString("Unknown command: " + cmdName);
        interp->setResult(obj);
        return false;
    } else {
        obj->setString(command->helpText());
        interp->setResult(obj);
        return true;
    }
}


/**
 * Returns help text.
 *
 * @return Help text.
 */
string
CmdHelp::helpText() const {
    return "Use help command to get help";
}
