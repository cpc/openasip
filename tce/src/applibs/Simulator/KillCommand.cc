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
 * @file KillCommand.cc
 *
 * Implementation of KillCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "KillCommand.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "SimulatorFrontend.hh"
#include "SimulatorInterpreterContext.hh"
#include "Exception.hh"
#include "SimulatorToolbox.hh"
#include "SimulatorTextGenerator.hh"

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
KillCommand::KillCommand() : 
    SimControlLanguageCommand("kill") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
KillCommand::~KillCommand() {
}

/**
 * Executes the "kill" command.
 *
 * Terminate the simulation. The program being simulated remains 
 * loaded and the simulation can be restarted from the beginning 
 * by means of command run. The Simulator will normally prompt the user 
 * for comfirmation before terminating the simulation.
 *
 * @param arguments No arguments needed.
 * @return Always true.
 * @exception NumberFormatException Is never thrown by this command.
 *
 */
bool 
KillCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    const int argumentCount = arguments.size() - 1;
    if (!checkArgumentCount(argumentCount, 0, 0)) {
        return false;
    } 

    if (!checkSimulationEnded()) {
        if (!checkSimulationStopped()) {
            return false;
        }
    }

    if (!interpreter()->lineReader()->confirmation(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_CONFIRM_KILL_SIMULATION).str())) {
        return false;
    }

    simulatorFrontend().killSimulation();
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
KillCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_KILL).str();
}

