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
 * @file IgnoreCommand.cc
 *
 * Implementation of IgnoreCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "IgnoreCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimValue.hh"
#include "StopPointManager.hh"

#include <iostream>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
IgnoreCommand::IgnoreCommand() : 
    SimControlLanguageCommand("ignore") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
IgnoreCommand::~IgnoreCommand() {
}

/**
 * Executes the "ignore" command.
 *
 * Modifies the ignore counts of breakpoints.
 *
 * @param arguments The count of steps (default is one step).
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 * @todo Use the count for the step.
 */
bool 
IgnoreCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    if (!checkArgumentCount(arguments.size() - 1, 2, 2)) {
        return false;
    } 

    if  (!checkPositiveIntegerArgument(arguments[1]) ||
         !checkPositiveIntegerArgument(arguments[2])) {
        return false;
    }

    if (!checkSimulationStopped()) {
        if (!checkSimulationInitialized()) {
            return false;
        }
    }

    const int breakpointHandle = arguments[1].integerValue();
    const int ignoreCount = arguments[2].integerValue();
    try {
        StopPointManager& stopPointManager =
            simulatorFrontend().stopPointManager();

        // just check that breakpoint is found with the handle
        stopPointManager.stopPointWithHandleConst(breakpointHandle);
        stopPointManager.setIgnore(breakpointHandle, ignoreCount);
        
        printBreakpointInfo(breakpointHandle);
    } catch (const InstanceNotFound&) {
        interpreter()->setError(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_BREAKPOINT_NOT_FOUND).str());
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
 * @todo Use SimulatorTextGenerator to get the help text.
 */
std::string 
IgnoreCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_IGNORE).str();
}
