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
