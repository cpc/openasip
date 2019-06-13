/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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

    virtual bool execute(const vector<DataObject>& arguments);
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
TestCmd::execute(const vector<DataObject>&) {
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
