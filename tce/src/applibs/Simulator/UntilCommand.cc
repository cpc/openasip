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
