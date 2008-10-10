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
 * @file TclInterpLineReader.cc
 *
 * A little test program for TclInterpreter and LineReader.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 */

#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::endl;
#include <vector>
using std::vector;

#include "TclInterpreter.hh"
#include "EditLineReader.hh"
#include "BaseLineReader.hh"
#include "LineReaderFactory.hh"
#include "CustomCommand.hh"
#include "DataObject.hh"
#include "Exception.hh"

/**
 * Test CustomCommand that asks user for confirmation.
 */
class TestCmd : public CustomCommand {
public:
    TestCmd();
    virtual ~TestCmd();

    virtual bool execute(const vector<DataObject>& arguments)
        throw (NumberFormatException);
    virtual string helpText() const;
};

/**
 * Constructor.
 */
TestCmd::TestCmd() : CustomCommand("foo") {
}

/**
 * Destructor.
 */
TestCmd::~TestCmd() {
}

/**
 * Executes foo command.
 *
 * @return True, if execution is succesfull, false otherwise.
 * @exception NumberFormatException Can not throw.
 */
bool
TestCmd::execute(const vector<DataObject>&) 
    throw (NumberFormatException) {
    
    ScriptInterpreter* interp = interpreter();
    LineReader* reader = interp->lineReader();
    DataObject* obj = new DataObject();
    
    if (reader->confirmation("Are you sure? ")) {
        obj->setString("foo executed");
        interp->setResult(obj);
        return true;
    } else {
        obj->setString("foo is not executed");
        interp->setResult(obj);
        return false;
    }
}

/**
 * Returns help string of the command.
 *
 * @return The help string.
 */
string
TestCmd::helpText() const {
    return "Useless test command";
}

/**
 * Test program for LineReader and TclInterpreter.
 *
 * Reads user inputs and interprets them until "quit" command is given.
 */
int main(int argc, char* argv[]) {
    TclInterpreter interpreter;
    LineReader* linereader = LineReaderFactory::lineReader();
    TestCmd* cmd = new TestCmd();
    interpreter.initialize(argc, argv, NULL, linereader);
    interpreter.addCustomCommand(cmd);
    linereader->initialize("(test) ");
    string command = "";
    bool cont = true;
    while (cont) {
        command = linereader->readLine();
        if (command == "quit\n") {
            cont = false;
        } else {
            interpreter.interpret(command);
            cout << interpreter.result() << endl;
        }
    }
    delete linereader;
    interpreter.finalize();
}
