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
 * @file BPCommand.cc
 *
 * Implementation of BPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "BPCommand.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"
#include "SimControlLanguageCommand.hh"
#include "StopPointManager.hh"
#include "Breakpoint.hh"

#include <iostream>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
BPCommand::BPCommand() : SimControlLanguageCommand("bp") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
BPCommand::~BPCommand() {
}

/**
 * Executes the "bp" command.
 *
 * A breakpoint stops the simulation whenever the Simulator reaches 
 * a certain point in the program. It is possible to add a condition to 
 * a breakpoint, to control when the Simulator must stop with increased 
 * precision. 
 *
 * @param arguments The address (and condition of the breakpoint).
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 */
bool 
BPCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    Breakpoint breakpoint(simulatorFrontend(), 0);
    if (!parseBreakpoint(arguments, breakpoint)) {
        
        return false;
    }

    StopPointManager& stopPointManager = 
        simulatorFrontend().stopPointManager();
    return printBreakpointInfo(stopPointManager.add(breakpoint));
}

/**
 * Returns the help text for this command.
 * 
 * Help text is searched from SimulatorTextGenerator.
 *
 * @return The help text.
 */
std::string 
BPCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_BREAK).str();
}
