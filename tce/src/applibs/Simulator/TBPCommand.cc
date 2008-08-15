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
 * @file TBPCommand.cc
 *
 * Implementation of TBPCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "TBPCommand.hh"
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
TBPCommand::TBPCommand() : SimControlLanguageCommand("tbp") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
TBPCommand::~TBPCommand() {
}

/**
 * Executes the "tbp" command.
 *
 * Sets a temporary breakpoint, which is automatically deleted after 
 * the first time it stops the simulation. The arguments args are the 
 * same as for the bp command. 
 *
 * @param arguments The address (and condition of the breakpoint).
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 */
bool 
TBPCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    Breakpoint breakpoint(simulatorFrontend(), 0);
    if (!parseBreakpoint(arguments, breakpoint)) {
        return false;
    }
    breakpoint.setDeletedAfterTriggered(true);

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
TBPCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_TBREAK).str();
}
