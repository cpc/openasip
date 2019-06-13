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
 * @file NextiCommand.cc
 *
 * Implementation of NextiCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
NextiCommand::execute(const std::vector<DataObject>& arguments) {
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
