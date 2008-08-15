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
 * @file NextiCommand.cc
 *
 * Implementation of NextiCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "NextiCommand.hh"
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
#include "Instruction.hh"

#include <iostream>

using namespace TTAProgram;

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
NextiCommand::NextiCommand() : 
    SimControlLanguageCommand("nexti") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
NextiCommand::~NextiCommand() {
}

/**
 * Executes the "nexti" command.
 *
 * Advances simulation to the next machine instructions in current 
 * procedure. If the instruction contains a function call, simulation 
 * proceeds until control returns from it, to the instruction past 
 * the function call. The count argument gives the number of machine 
 * instruction to simulate.
 *
 * @param arguments The count of steps (default is one step).
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 */
bool 
NextiCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {
    
    boost::timer time;    

    const int argumentCount = arguments.size() - 1;
    if (!checkArgumentCount(argumentCount, 0, 1)) {
        return false;
    } 

    if (!checkSimulationInitialized() && !checkSimulationStopped()) {
        return false;
    }

    unsigned int stepCount = 1;
    if (argumentCount == 1) { 
        if (!checkPositiveIntegerArgument(arguments[1])) {
            return false;
        }
        stepCount = arguments[1].integerValue();
    }        
    
    simulatorFrontend().next(stepCount);
        
    printNextInstruction();
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
NextiCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_NEXTI).str();
}
