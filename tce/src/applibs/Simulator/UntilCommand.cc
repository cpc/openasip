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
 * @file UntilCommand.cc
 *
 * Implementation of UntilCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "UntilCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimControlLanguageCommand.hh"
#include "SimValue.hh"
#include "Procedure.hh"

#include <iostream>
#include <boost/timer.hpp>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
UntilCommand::UntilCommand() : SimControlLanguageCommand("until") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
UntilCommand::~UntilCommand() {
}

/**
 * Executes the "until" command.
 *
 * Run until the program location specified by arg is reached and stop
 * before executing the instruction at specified address. Any valid 
 * argument that applies to command break is also a valid argument for until. 
 *
 * @param arguments The count of steps (default is one step).
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 */
bool 
UntilCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {
    
    boost::timer time;
    
    const int argumentCount = arguments.size() - 1;
    if (!checkArgumentCount(argumentCount, 0, 1)) {
        return false;
    } 

    if (!checkSimulationInitialized() && !checkSimulationStopped()) {
        return false;
    }

    try {
        const std::string argument = 
            ((argumentCount == 1)?(arguments[1].stringValue()):(""));

        const int instructionAddress = 
            parseInstructionAddressExpression(argument);
        simulatorFrontend().runUntil(instructionAddress);
        printNextInstruction();
        printSimulationTime();
    } catch (const IllegalParameters&) {
        return false;
    }
    return true;
}

/**
 * Returns the help text for this command.
 * 
 * Help text is searched from SimulatorTextGenerator.
 *
 * @return The help text.
 */
std::string 
UntilCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_UNTIL).str();
}
