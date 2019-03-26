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
 * @file QuitCommand.cc
 *
 * Implementation of QuitCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "QuitCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimulatorInterpreter.hh"

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
QuitCommand::QuitCommand(const std::string& commandString) : 
    CustomCommand(commandString) {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
QuitCommand::~QuitCommand() {
}

/**
 * Executes the "quit" command.
 *
 * This method does the uninitialization of the simulator.
 *
 * @param arguments No arguments needed.
 * @return Always true.
 * @exception NumberFormatException Is never thrown by this command.
 *
 */
bool 
QuitCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {
    const int argumentCount = arguments.size() - 1;
    if (!checkArgumentCount(argumentCount, 0, 0)) {
        return false;
    } 
    dynamic_cast<SimulatorInterpreter*>(interpreter())->setQuitCommandGiven();
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
QuitCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_QUIT).str();
}

