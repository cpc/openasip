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
 * @file CommandsCommand.cc
 *
 * Implementation of CommandsCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
