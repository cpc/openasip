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
 * @file RunCommand.cc
 *
 * Implementation of RunCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "RunCommand.hh"
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
RunCommand::RunCommand() : SimControlLanguageCommand("run") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
RunCommand::~RunCommand() {
}

/**
 * Executes the "run" command.
 *
 * Starts the simulation.
 *
 * @param arguments No arguments needed.
 * @return Always true.
 * @exception NumberFormatException Is never thrown by this command.
 *
 */
bool 
RunCommand::execute(const std::vector<DataObject>&)
    throw (NumberFormatException) {
        
    if (!checkSimulationNotAlreadyRunning() ||
        !checkSimulationInitialized()) {
        
        return false;
    }
    
    simulatorFrontend().run();
    
    printStopInformation();

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
RunCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_RUN).str();
}

