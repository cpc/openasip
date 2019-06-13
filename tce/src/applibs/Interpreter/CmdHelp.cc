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
 * @file CmdHelp.cc
 *
 * Declaration of CmdHelp class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
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
CmdHelp::execute(const std::vector<DataObject>& arguments) {
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
