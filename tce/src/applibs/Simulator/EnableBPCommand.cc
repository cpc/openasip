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
 * @file EnableBPCommand.cc
 *
 * Implementation of EnableBPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "EnableBPCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimValue.hh"
#include "StopPointManager.hh"
#include "StringTools.hh"

#include <iostream>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
EnableBPCommand::EnableBPCommand() : 
    SimControlLanguageCommand("enablebp") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
EnableBPCommand::~EnableBPCommand() {
}

/**
 * Executes the "enablebp" command.
 *
 * Enables the given breakpoints.
 *
 * @param arguments The handle(s) of the breakpoint(s) to enable.
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 * @todo Use the count for the step.
 */
bool 
EnableBPCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    if (!checkArgumentCount(arguments.size() - 1, 1, INT_MAX)) {
        return false;
    } 

    if (!checkSimulationStopped()) {
        if (!checkSimulationInitialized()) {
            return false;
        }
    }

    const std::string firstArgument = 
        StringTools::trim(StringTools::stringToLower(
                              arguments[1].stringValue()));
    
    bool enableOnce = false;
    bool temporary = false;
    int firstHandleIndex = 1;
    if (firstArgument == "once") {
        enableOnce = true;
        firstHandleIndex = 2;
    } else if (firstArgument == "delete") {
        temporary = true;
        firstHandleIndex = 2;
    }

    StopPointManager& breakpointManager =
        simulatorFrontend().stopPointManager();
    
    if (!verifyBreakpointHandles(arguments, firstHandleIndex)) {
        return false;
    }

    // enable (and modify the properties of) the given breakpoints
    for (size_t i = firstHandleIndex; i < arguments.size(); ++i) {
        const unsigned int breakpointHandle = arguments[i].integerValue();    
        breakpointManager.enable(breakpointHandle);
        if (enableOnce) {
            breakpointManager.enableOnceAndDisable(breakpointHandle);
        }
        if (temporary) {
            breakpointManager.enableOnceAndDelete(breakpointHandle);
        }
        printBreakpointInfo(breakpointHandle);
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
EnableBPCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_ENABLEBP).str();
}
